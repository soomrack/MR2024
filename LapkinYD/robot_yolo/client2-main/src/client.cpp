#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <QImage>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <cstdio>
#include <cstring>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include <opencv2/opencv.hpp>

#include "client.h"
#include "main_window.h"
#include "yolo_detector.h"
#include "ocr_detector.h"

int PORT_CLIENT = 8600;
const char *IP_CLIENT = "192.168.31.3";
const char *SERVER_IP = "192.168.31.149";

static std::ofstream g_logFile;
static std::mutex g_logMutex;
static std::atomic<uint64_t> g_frameCount{0};

static std::atomic<int> g_networkStressCounter{0};
static std::atomic<int> g_currentBitrate{2000};
static constexpr int BITRATE_ADJUST_THRESHOLD = 10;
static constexpr int BITRATE_MIN = 500;
static constexpr int BITRATE_MAX = 5000;
static constexpr int BITRATE_STEP = 500;
static constexpr int STRESS_COUNTER_DECAY = 50;

static void adaptBitrate(GstElement *encoder, int direction)
{
    // -1 = decrease, +1 = increase
    int newBitrate = g_currentBitrate.load() + direction * BITRATE_STEP;
    newBitrate = std::clamp(newBitrate, BITRATE_MIN, BITRATE_MAX);

    if (newBitrate != g_currentBitrate.load())
    {
        g_currentBitrate.store(newBitrate);
        g_object_set(encoder, "bitrate", newBitrate, NULL);

        std::string action = (direction < 0) ? "DECREASED" : "INCREASED";
        videoLog("BITRATE", action + " to " + std::to_string(newBitrate) +
                                " kbps (stress_counter was " + std::to_string(g_networkStressCounter.load()) + ")");
    }
}

static std::string videoTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto tt = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&tt), "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

static void videoLog(const std::string &level, const std::string &msg)
{
    std::lock_guard<std::mutex> lock(g_logMutex);
    std::string line = "[" + videoTimestamp() + "] [" + level + "] " + msg;
    if (g_logFile.is_open())
    {
        g_logFile << line << std::endl;
        g_logFile.flush();
    }
    std::cout << line << std::endl;
}

static GstElement *global_pipeline = NULL;

static std::mutex g_detMutex;
static std::vector<Detection> g_lastDetections;

typedef struct
{
    GstElement *pipeline;
    GMainLoop *main_loop;
} BusData;

typedef struct
{
    MainWindow *window;
    YoloDetector *detector;
    OcrDetector *ocr;
} SinkData;

static std::mutex g_ocrMutex;
static std::vector<OcrResult> g_lastOcrResults;

static gboolean bus_msg_handler(GstBus *bus, GstMessage *msg, gpointer user_data)
{
    BusData *data = (BusData *)user_data;
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_STATE_CHANGED:
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->pipeline))
        {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
            std::string stateMsg = std::string("Pipeline state: ") +
                                   gst_element_state_get_name(old_state) + " -> " +
                                   gst_element_state_get_name(new_state);
            videoLog("STATE", stateMsg);

            if (new_state == GST_STATE_PLAYING)
                videoLog("INFO", "Video stream STARTED on port " + std::to_string(PORT_CLIENT));
        }
        break;
    case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &err, &debug_info);
        videoLog("ERROR", std::string("From ") + GST_OBJECT_NAME(msg->src) + ": " + err->message);
        if (debug_info)
            videoLog("DEBUG", std::string("Debug: ") + debug_info);
        g_clear_error(&err);
        g_free(debug_info);
        g_main_loop_quit(data->main_loop);
        break;
    case GST_MESSAGE_EOS:
        videoLog("INFO", "End-Of-Stream reached. Total frames: " + std::to_string(g_frameCount.load()));
        g_main_loop_quit(data->main_loop);
        break;
    case GST_MESSAGE_WARNING:
        gst_message_parse_warning(msg, &err, &debug_info);
        videoLog("WARN", std::string("From ") + GST_OBJECT_NAME(msg->src) + ": " + err->message);
        g_clear_error(&err);
        g_free(debug_info);
        break;
    default:
        break;
    }
    return TRUE;
}

static GstFlowReturn on_new_sample_callback(GstAppSink *appsink, gpointer user_data)
{
    GstSample *sample = gst_app_sink_pull_sample(appsink);
    SinkData *data = (SinkData *)user_data;

    if (sample)
    {
        GstBuffer *buffer = gst_sample_get_buffer(sample);

        GstClockTime buf_pts = GST_BUFFER_PTS(buffer);
        static GstClockTime s_lastPts = 0;
        static int s_consecutiveLate = 0;

        // check network connection
        if (s_lastPts != 0 && buf_pts != GST_CLOCK_TIME_NONE)
        {
            GstClockTimeDiff diff = buf_pts - s_lastPts;

            // if diff is more 66 ms then frame is late
            if (diff > 66 * GST_MSECOND)
            {
                s_consecutiveLate++;

                // check 3 frame
                if (s_consecutiveLate >= 3)
                {
                    int counter = ++g_networkStressCounter;
                    if (counter >= BITRATE_ADJUST_THRESHOLD)
                    {
                        if (global_pipeline)
                        {
                            GstElement *encoder = gst_bin_get_by_name(GST_BIN(global_pipeline), "encoder");
                            if (encoder)
                            {
                                // decrease bitrate
                                adaptBitrate(encoder, -1);
                                gst_object_unref(encoder);
                            }
                        }

                        // reset counter
                        g_networkStressCounter.store(0);
                    }
                }
            }
            else
            {
                // every STRESS_COUNTER_DECAY frames decrease counter
                static int s_decayCounter = 0;
                if (++s_decayCounter >= STRESS_COUNTER_DECAY)
                {
                    s_decayCounter = 0;
                    int current = g_networkStressCounter.load();
                    if (current > 0)
                        g_networkStressCounter.fetch_sub(1);

                    // if net is stable and bitrate is law, so try to increase bitrate
                    if (current == 0 && g_currentBitrate.load() < BITRATE_MAX)
                    {
                        static int s_goodCounter = 0;
                        if (++s_goodCounter >= BITRATE_ADJUST_THRESHOLD * 2)
                        {
                            s_goodCounter = 0;
                            if (global_pipeline)
                            {
                                GstElement *encoder = gst_bin_get_by_name(GST_BIN(global_pipeline), "encoder");
                                if (encoder)
                                {

                                    // increase bitrate
                                    adaptBitrate(encoder, +1);
                                    gst_object_unref(encoder);
                                }
                            }
                        }
                    }
                }
                s_consecutiveLate = 0;
            }
        }
        s_lastPts = buf_pts;

        GstMapInfo map;

        if (gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            GstCaps *caps = gst_sample_get_caps(sample);
            GstStructure *structure = gst_caps_get_structure(caps, 0);
            int width = 0, height = 0;
            gst_structure_get_int(structure, "width", &width);
            gst_structure_get_int(structure, "height", &height);

            if (width > 0 && height > 0)
            {
                uint64_t frameNum = ++g_frameCount;

                if (frameNum == 1)
                    videoLog("INFO", "First frame received: " +
                                         std::to_string(width) + "x" + std::to_string(height));

                if (frameNum % 300 == 0)
                    videoLog("INFO", "Frames received: " + std::to_string(frameNum));

                int skip = g_displaySkip.load();
                if (frameNum % (uint64_t)skip != 0)
                {
                    gst_buffer_unmap(buffer, &map);
                    gst_sample_unref(sample);
                    return GST_FLOW_OK;
                }

                // gst give RGB out
                cv::Mat frameRGB(height, width, CV_8UC3, map.data);

                // yolo
                if (data->detector && data->detector->isLoaded() && frameNum % (uint64_t)(skip * 5) == 0)
                {
                    auto detections = data->detector->detect(frameRGB);
                    {
                        std::lock_guard<std::mutex> lock(g_detMutex);
                        g_lastDetections = detections;
                    }
                    // logging last 30 frames
                    if (frameNum % (uint64_t)(skip * 30) == 0)
                    {
                        std::string detLog = "Frame " + std::to_string(frameNum) +
                                             " skip=" + std::to_string(skip) +
                                             " detections: " + std::to_string(detections.size());
                        for (const auto &d : detections)
                            detLog += " | " + data->detector->className(d.classId) +
                                      " " + std::to_string((int)(d.confidence * 100)) + "%";
                        videoLog("YOLO", detLog);
                    }
                }

                // Рисуем последние YOLO детекции на каждом кадре
                {
                    std::lock_guard<std::mutex> lock(g_detMutex);
                    if (!g_lastDetections.empty())
                        data->detector->drawDetections(frameRGB, g_lastDetections);
                }

                // OCR — каждые 15 кадров
                if (data->ocr && data->ocr->isLoaded() && frameNum % 15 == 0)
                {
                    auto ocrRes = data->ocr->detect(frameRGB);
                    {
                        std::lock_guard<std::mutex> lock(g_ocrMutex);
                        g_lastOcrResults = ocrRes;
                    }
                    for (const auto &r : ocrRes)
                        videoLog("OCR", "Digit: " + r.text +
                                            " conf=" + std::to_string((int)r.confidence) + "%");
                }

                // Рисуем OCR результаты
                {
                    std::lock_guard<std::mutex> lock(g_ocrMutex);
                    if (!g_lastOcrResults.empty() && data->ocr)
                        data->ocr->drawResults(frameRGB, g_lastOcrResults);
                }

                QImage image(frameRGB.data, width, height,
                             (int)frameRGB.step,
                             QImage::Format_RGB888);

                // Передаём копию в Qt (оригинальный буфер принадлежит gst)
                data->window->updateFrame(image.copy());
            }

            gst_buffer_unmap(buffer, &map);
        }
        gst_sample_unref(sample);
    }

    return GST_FLOW_OK;
}

GStreamerClient::GStreamerClient(MainWindow *window, const std::string &yoloModel)
    : m_window(window), m_detector(yoloModel)
{
}

GStreamerClient::~GStreamerClient()
{
}

int GStreamerClient::listen()
{
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    {
        std::lock_guard<std::mutex> lock(g_logMutex);
        g_logFile.open("video_client.log", std::ios::app);
    }
    videoLog("INFO", "Video client started");
    videoLog("INFO", "Listening on port " + std::to_string(PORT_CLIENT));

    gst_init(0, 0);

    GstElement *pipeline = gst_pipeline_new("client_pipeline");
    GstElement *udpsrc = gst_element_factory_make("udpsrc", "source");
    GstElement *rtpjpegdepay = gst_element_factory_make("rtpjpegdepay", "depay");

    GstElement *tee = gst_element_factory_make("tee", "tee");
    GstElement *display_queue = gst_element_factory_make("queue", "display_queue");
    GstElement *record_queue = gst_element_factory_make("queue", "record_queue");

    GstElement *jpegdec = gst_element_factory_make("jpegdec", "decoder");
    GstElement *videoconvert = gst_element_factory_make("videoconvert", "convert");

    GstElement *appsink = gst_element_factory_make("appsink", "appsink");

    GstElement *record_convert = gst_element_factory_make("videoconvert", "record_convert");
    GstElement *x264enc = gst_element_factory_make("x264enc", "encoder");
    GstElement *mp4mux = gst_element_factory_make("mp4mux", "mp4mux");
    GstElement *filesink = gst_element_factory_make("filesink", "filesink");

    if (!pipeline || !udpsrc || !rtpjpegdepay || !tee || !display_queue || !record_queue ||
        !jpegdec || !videoconvert || !appsink || !record_convert || !x264enc ||
        !mp4mux || !filesink)
    {
        videoLog("ERROR", "Not all GStreamer elements could be created");
        return -1;
    }

    GstCaps *src_caps = gst_caps_new_simple("application/x-rtp",
                                            "media", G_TYPE_STRING, "video",
                                            "clock-rate", G_TYPE_INT, 90000,
                                            "encoding-name", G_TYPE_STRING, "JPEG",
                                            "payload", G_TYPE_INT, 26,
                                            NULL);
    g_object_set(udpsrc,
                 "port", PORT_CLIENT,
                 "buffer-size", 2097152,
                 "caps", src_caps,
                 NULL);
    gst_caps_unref(src_caps);

    g_object_set(appsink,
                 "emit-signals", TRUE,
                 "sync", FALSE,
                 "max-buffers", 1,
                 "drop", TRUE,
                 NULL);

    GstCaps *appsink_caps = gst_caps_from_string("video/x-raw,format=RGB");
    g_object_set(appsink, "caps", appsink_caps, NULL);
    gst_caps_unref(appsink_caps);

    g_object_set(filesink,
                 "location", "client_recording.mp4",
                 "sync", FALSE,
                 NULL);

    g_object_set(x264enc,
                 "bitrate", 2000,
                 "speed-preset", 4,
                 "key-int-max", 30,
                 NULL);

    gst_bin_add_many(GST_BIN(pipeline),
                     udpsrc, rtpjpegdepay, tee,
                     display_queue, record_queue,
                     jpegdec, videoconvert, appsink,
                     record_convert, x264enc, mp4mux, filesink,
                     NULL);

    if (!gst_element_link_many(udpsrc, rtpjpegdepay, jpegdec, videoconvert, tee, NULL))
    {
        videoLog("ERROR", "Failed to link source -> tee chain");
        gst_object_unref(pipeline);
        return -1;
    }

    GstPad *tee_display_pad = gst_element_request_pad_simple(tee, "src_%u");
    GstPad *display_queue_sink = gst_element_get_static_pad(display_queue, "sink");
    if (!tee_display_pad || !display_queue_sink ||
        gst_pad_link(tee_display_pad, display_queue_sink) != GST_PAD_LINK_OK)
    {
        videoLog("ERROR", "Failed to link tee to display branch");
        gst_object_unref(tee_display_pad);
        gst_object_unref(display_queue_sink);
        gst_object_unref(pipeline);
        return -1;
    }
    gst_object_unref(tee_display_pad);
    gst_object_unref(display_queue_sink);

    if (!gst_element_link_many(display_queue, appsink, NULL))
    {
        videoLog("ERROR", "Failed to link display branch");
        gst_object_unref(pipeline);
        return -1;
    }

    GstPad *tee_record_pad = gst_element_request_pad_simple(tee, "src_%u");
    GstPad *record_queue_sink = gst_element_get_static_pad(record_queue, "sink");
    if (!tee_record_pad || !record_queue_sink ||
        gst_pad_link(tee_record_pad, record_queue_sink) != GST_PAD_LINK_OK)
    {
        videoLog("ERROR", "Failed to link tee to recording branch");
        gst_object_unref(tee_record_pad);
        gst_object_unref(record_queue_sink);
        gst_object_unref(pipeline);
        return -1;
    }
    gst_object_unref(tee_record_pad);
    gst_object_unref(record_queue_sink);

    GstCaps *record_caps = gst_caps_from_string("video/x-raw,format=I420");
    g_object_set(record_convert, "caps", record_caps, NULL);
    gst_caps_unref(record_caps);

    if (!gst_element_link_many(record_queue, record_convert, x264enc, mp4mux, filesink, NULL))
    {
        videoLog("ERROR", "Failed to link recording branch");
        gst_object_unref(pipeline);
        return -1;
    }

    SinkData *sinkData = new SinkData{this->m_window, &this->m_detector, &this->m_ocr};
    g_signal_connect(appsink, "new-sample", G_CALLBACK(on_new_sample_callback), sinkData);

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);

    BusData *bus_data = g_new0(BusData, 1);
    bus_data->pipeline = pipeline;
    bus_data->main_loop = main_loop;

    ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        videoLog("ERROR", "Unable to set pipeline to PAUSED");
        gst_object_unref(pipeline);
        g_free(bus_data);
        return -1;
    }

    g_usleep(100000);

    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        videoLog("ERROR", "Unable to set pipeline to PLAYING");
        gst_object_unref(pipeline);
        g_free(bus_data);
        return -1;
    }

    g_print("Client pipeline started. Recording to 'client_recording.mp4'...\n");
    g_print("Waiting for video stream on port %d...\n", PORT_CLIENT);
    videoLog("INFO", "Pipeline started, recording enabled");

    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_msg_handler, bus_data);

    g_main_loop_run(main_loop);

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(main_loop);
    g_free(bus_data);

    videoLog("INFO", "Video client stopped. Total frames: " +
                         std::to_string(g_frameCount.load()));
    videoLog("INFO", "Recording saved to 'client_recording.mp4'");
    {
        std::lock_guard<std::mutex> lock(g_logMutex);
        g_logFile.close();
    }

    return 0;
}

int GStreamerClient::run_listen()
{
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
    return gst_macos_main((GstMainFunc)this->listen, NULL);
#else
    return this->listen();
#endif
}