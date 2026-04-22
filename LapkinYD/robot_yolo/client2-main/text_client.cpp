#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

class TextClient
{
private:
    GstElement *send_pipeline;
    GstElement *recv_pipeline;
    GstElement *appsrc;
    GstElement *appsink;
    GstBus *send_bus;
    GstBus *recv_bus;
    GMainLoop *loop;
    std::string host;
    int sendPort;
    int receivePort;

    // logging
    std::ofstream logFile;
    std::mutex logMutex;
    std::string logFilename;

    // robot command file
    std::string commandFilename;
    std::mutex commandMutex;
    bool running;

    std::string getTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    void writeLog(const std::string &level, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open())
        {
            logFile << "[" << getTimestamp() << "] [" << level << "] " << message << std::endl;
            logFile.flush();
        }

        std::cout << "[" << level << "] " << message << std::endl;
    }

    std::string readCommandFromFile()
    {
        std::lock_guard<std::mutex> lock(commandMutex);
        std::string command;

        try
        {
            std::ifstream file(commandFilename, std::ios::in);

            if (!file.is_open())
            {
                writeLog("WARN", "Cannot open command file (may be locked): " + commandFilename);
                return "";
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            command = buffer.str();
            file.close();

            // trim whitespace and newlines
            size_t start = command.find_first_not_of(" \t\n\r");
            size_t end = command.find_last_not_of(" \t\n\r");

            // check file is empty
            if (start == std::string::npos)
            {
                return "";
            }

            command = command.substr(start, end - start + 1);

            // clear file after reading it
            if (!command.empty())
            {
                std::ofstream clearFile(commandFilename, std::ios::out | std::ios::trunc);
                if (clearFile.is_open())
                {
                    clearFile.close();
                    writeLog("INFO", "Command file cleared after reading");
                }
                else
                {
                    writeLog("WARN", "Could not clear command file");
                }
            }
        }
        catch (const std::exception &e)
        {
            writeLog("ERROR", "Error reading command file: " + std::string(e.what()));
            return "";
        }

        return command;
    }

    // gst bus function
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
    {
        TextClient *client = static_cast<TextClient *>(data);

        switch (GST_MESSAGE_TYPE(msg))
        {
        case GST_MESSAGE_EOS:
            client->writeLog("INFO", "End of stream");
            break;
        case GST_MESSAGE_ERROR:
        {
            GError *err;
            gchar *debug;
            gst_message_parse_error(msg, &err, &debug);
            std::string errorMsg = std::string("Error: ") + err->message;
            client->writeLog("ERROR", errorMsg);
            g_error_free(err);
            g_free(debug);
            break;
        }
        default:
            break;
        }
        return TRUE;
    }

    // gst callback receiving data from server
    static GstFlowReturn on_new_sample(GstAppSink *sink, gpointer data)
    {
        TextClient *client = static_cast<TextClient *>(data);
        client->receiveData(GST_ELEMENT(sink));
        return GST_FLOW_OK;
    }

    void receiveData(GstElement *sink)
    {
        GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
        if (sample)
        {
            GstBuffer *buffer = gst_sample_get_buffer(sample);
            GstMapInfo map;

            if (gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                std::string text(reinterpret_cast<char *>(map.data), map.size);

                // remove null bytes and newlines if it is in the text
                size_t end = text.find('\n');
                if (end != std::string::npos)
                {
                    text = text.substr(0, end);
                }
                size_t null_pos = text.find('\0');
                if (null_pos != std::string::npos)
                {
                    text = text.substr(0, null_pos);
                }
                if (!text.empty())
                {
                    writeLog("RECV", "From server: " + text);
                }
                gst_buffer_unmap(buffer, &map);
            }
            gst_sample_unref(sample);
        }
    }

    void commandMonitorThread()
    {
        writeLog("INFO", "Command monitor thread started");

        while (running)
        {
            std::string command = readCommandFromFile();

            if (!command.empty())
            {
                if (command == "quit" || command == "exit")
                {
                    writeLog("INFO", "Quit command received from file");
                    running = false;
                    if (loop && g_main_loop_is_running(loop))
                    {
                        g_main_loop_quit(loop);
                    }
                    break;
                }

                sendText(command);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        writeLog("INFO", "Command monitor thread stopped");
    }

public:
    TextClient(const std::string &h = "127.0.0.1", int sPort = 5000, int rPort = 5001,
               const std::string &logFile = "client.log",
               const std::string &cmdFile = "command.txt")
        : host(h), sendPort(sPort), receivePort(rPort), logFilename(logFile),
          commandFilename(cmdFile),
          send_pipeline(nullptr), recv_pipeline(nullptr),
          appsrc(nullptr), appsink(nullptr),
          send_bus(nullptr), recv_bus(nullptr), loop(nullptr),
          running(false) {}

    ~TextClient()
    {
        running = false;

        if (logFile.is_open())
        {
            writeLog("INFO", "Client shutting down");
            logFile.close();
        }
        if (loop && g_main_loop_is_running(loop))
        {
            g_main_loop_quit(loop);
        }
        if (send_pipeline)
        {
            gst_element_set_state(send_pipeline, GST_STATE_NULL);
            gst_object_unref(send_pipeline);
        }
        if (recv_pipeline)
        {
            gst_element_set_state(recv_pipeline, GST_STATE_NULL);
            gst_object_unref(recv_pipeline);
        }
        if (loop)
        {
            g_main_loop_unref(loop);
        }
    }

    bool init()
    {
        gst_init(nullptr, nullptr);

        logFile.open(logFilename, std::ios::app);
        if (!logFile.is_open())
        {
            std::cerr << "Warning: Could not open log file: " << logFilename << std::endl;
        }
        else
        {
            writeLog("INFO", "=== Client Started ===");
            writeLog("INFO", "Log file: " + logFilename);
            writeLog("INFO", "Command file: " + commandFilename);
        }

        // pipeline (client -> server on receivePort)
        std::string send_pipeline_str =
            "appsrc name=src emit-signals=false is-live=true "
            "format=time caps=application/x-text ! "
            "tcpclientsink host=" +
            host + " port=" + std::to_string(receivePort) +
            " sync=false async=false";

        GError *error = nullptr;
        send_pipeline = gst_parse_launch(send_pipeline_str.c_str(), &error);

        if (error)
        {
            std::string errorMsg = std::string("Error creating send pipeline: ") + error->message;
            writeLog("ERROR", errorMsg);
            g_error_free(error);
            return false;
        }

        appsrc = gst_bin_get_by_name(GST_BIN(send_pipeline), "src");
        if (!appsrc)
        {
            writeLog("ERROR", "Failed to get appsrc");
            return false;
        }

        g_object_set(appsrc, "format", GST_FORMAT_TIME, nullptr);
        GstCaps *caps = gst_caps_new_simple("application/x-text", nullptr);
        g_object_set(appsrc, "caps", caps, nullptr);
        gst_caps_unref(caps);

        // pipeline (server -> client on sendPort)
        std::string receive_pipeline_str =
            "tcpclientsrc host=" + host + " port=" + std::to_string(sendPort) + " ! "
                                                                                "appsink name=sink emit-signals=true sync=false";

        receive_pipeline = gst_parse_launch(recv_pipeline_str.c_str(), &error);

        if (error)
        {
            std::string errorMsg = std::string("Error creating receive pipeline: ") + error->message;
            writeLog("ERROR", errorMsg);
            g_error_free(error);
            return false;
        }

        appsink = gst_bin_get_by_name(GST_BIN(receive_pipeline), "sink");
        if (!appsink)
        {
            writeLog("ERROR", "Failed to get appsink");
            return false;
        }

        GstAppSinkCallbacks callbacks = {nullptr, nullptr, on_new_sample};
        gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, this, nullptr);

        send_bus = gst_pipeline_get_bus(GST_PIPELINE(send_pipeline));
        gst_bus_add_watch(send_bus, bus_call, this);

        recv_bus = gst_pipeline_get_bus(GST_PIPELINE(receive_pipeline));
        gst_bus_add_watch(recv_bus, bus_call, this);

        writeLog("INFO", "Client initialized");
        writeLog("INFO", "Send port (client->server): " + std::to_string(receivePort));
        writeLog("INFO", "Receive port (server->client): " + std::to_string(sendPort));
        return true;
    }

    bool sendText(const std::string &text)
    {
        if (!appsrc)
        {
            writeLog("ERROR", "appsrc not initialized");
            return false;
        }

        std::string framed_text = text + "\n";

        GstBuffer *buffer = gst_buffer_new_allocate(nullptr, framed_text.size(), nullptr);
        if (!buffer)
        {
            writeLog("ERROR", "Failed to create buffer");
            return false;
        }

        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_WRITE);
        memcpy(map.data, framed_text.c_str(), framed_text.size());
        gst_buffer_unmap(buffer, &map);

        GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(appsrc), buffer);

        if (ret != GST_FLOW_OK)
        {
            std::string errorMsg = "Error sending buffer: " + std::to_string(ret);
            writeLog("ERROR", errorMsg);
            return false;
        }

        writeLog("SEND", "To server: " + text);
        return true;
    }

    void run()
    {
        gst_element_set_state(send_pipeline, GST_STATE_PLAYING);
        gst_element_set_state(recv_pipeline, GST_STATE_PLAYING);

        writeLog("INFO", "========================================");
        writeLog("INFO", "Client connecting to " + host);
        writeLog("INFO", "Command monitoring enabled: " + commandFilename);
        writeLog("INFO", "========================================");

        loop = g_main_loop_new(nullptr, FALSE);
        running = true;

        // start command file to monitor thread
        std::thread cmdThread(&TextClient::commandMonitorThread, this);

        std::cout << "\nClient running. Write commands to " << commandFilename << std::endl;
        std::cout << "Write 'quit' or 'exit' to stop the client." << std::endl;

        g_main_loop_run(loop);

        running = false;

        if (cmdThread.joinable())
        {
            cmdThread.join();
        }

        std::cout << "Client stopped" << std::endl;
    }
};

int main(int argc, char *argv[])
{
    std::string host = "192.168.31.149";
    int sendPort = 5000;
    int receivePort = 5001;
    std::string logFile = "client.log";
    std::string commandFile = "command.txt";

    if (argc > 1)
        host = argv[1];
    if (argc > 2)
        sendPort = std::stoi(argv[2]);
    if (argc > 3)
        receivePort = std::stoi(argv[3]);
    if (argc > 4)
        logFile = argv[4];
    if (argc > 5)
        commandFile = argv[5];

    TextClient client(host, sendPort, receivePort, logFile, commandFile);

    if (!client.init())
    {
        std::cerr << "Error initializing client" << std::endl;
        return -1;
    }

    client.run();

    return 0;
}