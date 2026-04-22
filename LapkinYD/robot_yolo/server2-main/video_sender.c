#include <gst/gst.h>
#include <gst/rtp/gstrtcpbuffer.h>
#include <signal.h>
#include <stdio.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

const char *IP_CLIENT = "192.168.31.3";
int PORT_CLIENT = 8600;

static GstElement *global_pipeline = NULL;

typedef struct
{
    GstElement *pipeline;
    GMainLoop *main_loop;
} BusData;

static void sigint_handler(int sig)
{
    if (global_pipeline)
    {
        g_print("\nSending EOS to finalize recording...\n");
        GstElement *source = gst_bin_get_by_name(GST_BIN(global_pipeline), "source");
        if (source)
        {
            gst_element_send_event(source, gst_event_new_eos());
            gst_object_unref(source);
        }
    }
}

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
            g_print("Pipeline state: %s -> %s (pending: %s)\n",
                    gst_element_state_get_name(old_state),
                    gst_element_state_get_name(new_state),
                    gst_element_state_get_name(pending_state));
        }
        break;
    case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error from %s: %s\n",
                   GST_OBJECT_NAME(msg->src), err->message);
        g_clear_error(&err);
        g_free(debug_info);
        g_main_loop_quit(data->main_loop);
        break;
    case GST_MESSAGE_EOS:
        g_print("End-Of-Stream reached.\n");
        g_main_loop_quit(data->main_loop);
        break;
    default:
        break;
    }
    return TRUE;
}

int tutorial_main(int argc, char *argv[])
{
    GstElement *pipeline, *source, *capsfilter, *videoscale, *videoconverter,
        *x264enc_stream, *rtppay, *udpsink, *tee,
        *record_queue, *mp4mux, *filesink, *x264enc_record;
    GstElement *stream_queue;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    GstPad *tee_src_pad1, *tee_src_pad2, *queue_sink_pad, *rtppay_sink_pad;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
#ifdef _WIN32
    source = gst_element_factory_make("ksvideosrc", "source");
#else
    source = gst_element_factory_make("libcamerasrc", "source");
#endif

    rtppay = gst_element_factory_make("rtph264pay", "rtppay");
    udpsink = gst_element_factory_make("udpsink", "udpsink");
    capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    videoscale = gst_element_factory_make("videoscale", "videoscale");
    videoconverter = gst_element_factory_make("videoconvert", "videoconvert");
    GstElement *rtpbin = gst_element_factory_make("rtpbin", "rtpbin");

    x264enc_stream = gst_element_factory_make("x264enc", "x264enc_stream");
    x264enc_record = gst_element_factory_make("x264enc", "x264enc_record");

    tee = gst_element_factory_make("tee", "tee");
    record_queue = gst_element_factory_make("queue", "record_queue");
    stream_queue = gst_element_factory_make("queue", "stream_queue");
    mp4mux = gst_element_factory_make("mp4mux", "mp4mux");
    filesink = gst_element_factory_make("filesink", "filesink");

    GstCaps *caps = gst_caps_from_string("video/x-raw,format=I420,width=1280,height=720,framerate=30/1");
    g_object_set(capsfilter, "caps", caps, NULL);
    gst_caps_unref(caps);

    pipeline = gst_pipeline_new("adaptive_pipeline");

    if (!pipeline || !source || !capsfilter || !udpsink || !rtppay ||
        !videoscale || !x264enc_stream || !x264enc_record || !videoconverter ||
        !tee || !record_queue || !stream_queue || !mp4mux || !filesink || !rtpbin)
    {
        g_printerr("Not all elements could be created.\n");
        if (!stream_queue)
            g_printerr("Failed to create stream_queue\n");
        if (!x264enc_stream)
            g_printerr("Failed to create x264enc_stream\n");
        if (!x264enc_record)
            g_printerr("Failed to create x264enc_record\n");
        return -1;
    }

    gst_bin_add_many(GST_BIN(pipeline),
                     source, videoconverter, videoscale, capsfilter,
                     tee, rtpbin,
                     stream_queue, x264enc_stream, rtppay, udpsink,
                     record_queue, x264enc_record, mp4mux, filesink,
                     NULL);

#ifdef _WIN32
    g_object_set(source, "device-index", 0, NULL);
#endif

    g_object_set(rtppay,
                 "pt", 96,
                 "config-interval", 1,
                 "mtu", 1400,
                 NULL);

    g_object_set(udpsink,
                 "host", IP_CLIENT,
                 "port", PORT_CLIENT,
                 "sync", FALSE,
                 "async", FALSE,
                 "buffer-size", 4194304,
                 NULL);

    g_object_set(x264enc_stream,
                 "bitrate", 1500,
                 "speed-preset", 1,
                 "tune", 0x00000004,
                 "key-int-max", 60,
                 NULL);

    g_object_set(x264enc_record,
                 "bitrate", 2500,
                 "speed-preset", 2,
                 "tune", 0x00000004,
                 "key-int-max", 60,
                 NULL);

    g_object_set(filesink, "location", "recording.mp4", NULL);

    if (!gst_element_link_many(source, videoconverter, videoscale, capsfilter, tee, NULL))
    {
        g_printerr("Failed to link source -> tee chain.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (!gst_element_link_many(stream_queue, x264enc_stream, rtppay, udpsink, NULL))
    {
        g_printerr("Failed to link stream chain.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (!gst_element_link_many(record_queue, x264enc_record, mp4mux, filesink, NULL))
    {
        g_printerr("Failed to link record chain.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    tee_src_pad1 = gst_element_request_pad_simple(tee, "src_%u");
    if (!tee_src_pad1)
    {
        g_printerr("Failed to get first tee src pad\n");
        gst_object_unref(pipeline);
        return -1;
    }

    queue_sink_pad = gst_element_get_static_pad(stream_queue, "sink");
    if (!queue_sink_pad)
    {
        g_printerr("Failed to get stream_queue sink pad\n");
        gst_object_unref(tee_src_pad1);
        gst_object_unref(pipeline);
        return -1;
    }

    if (gst_pad_link(tee_src_pad1, queue_sink_pad) != GST_PAD_LINK_OK)
    {
        g_printerr("Failed to link tee to stream_queue\n");
        gst_object_unref(tee_src_pad1);
        gst_object_unref(queue_sink_pad);
        gst_object_unref(pipeline);
        return -1;
    }

    gst_object_unref(tee_src_pad1);
    gst_object_unref(queue_sink_pad);

    tee_src_pad2 = gst_element_request_pad_simple(tee, "src_%u");
    if (!tee_src_pad2)
    {
        g_printerr("Failed to get second tee src pad\n");
        gst_object_unref(pipeline);
        return -1;
    }

    queue_sink_pad = gst_element_get_static_pad(record_queue, "sink");
    if (!queue_sink_pad)
    {
        g_printerr("Failed to get record_queue sink pad\n");
        gst_object_unref(tee_src_pad2);
        gst_object_unref(pipeline);
        return -1;
    }

    if (gst_pad_link(tee_src_pad2, queue_sink_pad) != GST_PAD_LINK_OK)
    {
        g_printerr("Failed to link tee to record_queue\n");
        gst_object_unref(tee_src_pad2);
        gst_object_unref(queue_sink_pad);
        gst_object_unref(pipeline);
        return -1;
    }

    gst_object_unref(tee_src_pad2);
    gst_object_unref(queue_sink_pad);

    global_pipeline = pipeline;
    signal(SIGINT, sigint_handler);

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);

    BusData *bus_data = g_new(BusData, 1);
    bus_data->pipeline = pipeline;
    bus_data->main_loop = main_loop;

    /* Start playing */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

/* Modify the source's properties */
#ifndef _WIN32
    g_object_set(source, "pattern", 0, NULL); // Linux
#endif

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_msg_handler, bus_data);

    // run event loop
    g_main_loop_run(main_loop);

    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(main_loop);
    return 0;
}

int main(int argc, char *argv[])
{
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
    return gst_macos_main((GstMainFunc)tutorial_main, argc, argv, NULL);
#else
    return tutorial_main(argc, argv);
#endif
}