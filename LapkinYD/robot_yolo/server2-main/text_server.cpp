#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

class TextServer
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
    std::atomic<bool> running{true};
    std::atomic<bool> clientConnected{false};

    std::ofstream logFile;
    std::mutex logMutex;
    std::string logFilename;
    std::string commandFilename;
    std::mutex commandFileMutex;

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

    void onClientConnected()
    {
        bool expected = false;
        if (clientConnected.compare_exchange_strong(expected, true))
        {
            writeLog("CONNECTION", "CLIENT CONNECTED!");
        }
    }

    void onClientDisconnected()
    {
        bool expected = true;
        if (clientConnected.compare_exchange_strong(expected, false))
        {
            writeLog("CONNECTION", "CLIENT DISCONNECTED!");
        }
    }

    static gboolean send_bus_call(GstBus *bus, GstMessage *msg, gpointer data)
    {
        TextServer *server = static_cast<TextServer *>(data);

        switch (GST_MESSAGE_TYPE(msg))
        {
        case GST_MESSAGE_EOS:
            server->writeLog("INFO", "End of stream (send)");
            break;
        case GST_MESSAGE_ERROR:
        {
            GError *err;
            gchar *debug;
            gst_message_parse_error(msg, &err, &debug);
            std::string errorMsg = std::string("Send Error: ") + err->message;
            std::string debugInfo = debug ? std::string(debug) : "no debug info";
            server->writeLog("ERROR", errorMsg);
            server->writeLog("ERROR", "Debug: " + debugInfo);
            g_error_free(err);
            g_free(debug);
            break;
        }
        case GST_MESSAGE_ELEMENT:
        {
            const GstStructure *s = gst_message_get_structure(msg);
            if (s)
            {
                const gchar *name = gst_structure_get_name(s);
                if (name && g_strcmp0(name, "GstTCPServerSinkClientAccepted") == 0)
                {
                    server->onClientConnected();
                }
                else if (name && g_strcmp0(name, "GstTCPServerSinkClientClosed") == 0)
                {
                    server->onClientDisconnected();
                }
            }
            break;
        }
        default:
            break;
        }
        return TRUE;
    }

    static gboolean recv_bus_call(GstBus *bus, GstMessage *msg, gpointer data)
    {
        TextServer *server = static_cast<TextServer *>(data);

        switch (GST_MESSAGE_TYPE(msg))
        {
        case GST_MESSAGE_EOS:
            server->writeLog("INFO", "End of stream (receive)");
            server->onClientDisconnected();
            break;
        case GST_MESSAGE_ERROR:
        {
            GError *err;
            gchar *debug;
            gst_message_parse_error(msg, &err, &debug);
            std::string errorMsg = std::string("Receive Error: ") + err->message;
            std::string debugInfo = debug ? std::string(debug) : "no debug info";
            server->writeLog("ERROR", errorMsg);
            server->writeLog("ERROR", "Debug: " + debugInfo);
            g_error_free(err);
            g_free(debug);
            server->onClientDisconnected();
            break;
        }
        case GST_MESSAGE_ELEMENT:
        {
            const GstStructure *s = gst_message_get_structure(msg);
            if (s)
            {
                const gchar *name = gst_structure_get_name(s);
                if (name && g_strcmp0(name, "GstTCPServerSrcClientAccepted") == 0)
                {
                    server->onClientConnected();
                }
                else if (name && g_strcmp0(name, "GstTCPServerSrcClientClosed") == 0)
                {
                    server->onClientDisconnected();
                }
            }
            break;
        }
        default:
            break;
        }
        return TRUE;
    }

    static GstFlowReturn on_new_sample(GstAppSink *sink, gpointer data)
    {
        TextServer *server = static_cast<TextServer *>(data);
        server->receiveData(GST_ELEMENT(sink));
        return GST_FLOW_OK;
    }

    void receiveData(GstElement *sink)
    {
        onClientConnected();

        GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
        if (sample)
        {
            GstBuffer *buffer = gst_sample_get_buffer(sample);
            GstMapInfo map;

            if (gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                std::string text(reinterpret_cast<char *>(map.data), map.size);
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
                    writeLog("RECV", "From client: " + text);
                    writeCommand(text);
                }
                gst_buffer_unmap(buffer, &map);
            }
            gst_sample_unref(sample);
        }
    }

    void writeCommand(const std::string &command)
    {
        writeLog("DEBUG", ">>> ENTER writeCommand, command='" + command + "'");

        std::string output;

        if (command == "Forward")
            output = "W";
        else if (command == "Back")
            output = "S";
        else if (command == "Left")
            output = "A";
        else if (command == "Right")
            output = "D";
        else if (command == "Stop")
            output = "X";
        else if (command == "Quit")
            output = "Q";
        else
        {
            writeLog("WARN", "Command NOT matched: '" + command + "'");
            return;
        }

        writeLog("DEBUG", ">>> Attempting to write '" + output + "' to file: " + commandFilename);

        std::lock_guard<std::mutex> lock(commandFileMutex);

        std::string tmpFilename = commandFilename + ".tmp";
        try
        {
            std::ofstream cmdFile(tmpFilename, std::ios::trunc);
            if (!cmdFile.is_open())
            {
                writeLog("ERROR", "Failed to open tmp file");
                return;
            }
            cmdFile << output;
            cmdFile.flush();
            cmdFile.close();
            if (std::rename(tmpFilename.c_str(), commandFilename.c_str()) != 0)
            {
                writeLog("ERROR", "rename() failed");
                return;
            }
            writeLog("DEBUG", ">>> SUCCESS: wrote '" + output + "' to " + commandFilename);
        }
        catch (const std::exception &e)
        {
            writeLog("ERROR", "Exception: " + std::string(e.what()));
        }
    }

public:
    TextServer(const std::string &h = "0.0.0.0", int sPort = 5000, int rPort = 5001,
               const std::string &logFile = "server.log", const std::string &commandFile = "command.txt")
        : host(h), sendPort(sPort), receivePort(rPort), logFilename(logFile),
          commandFilename(commandFile),
          send_pipeline(nullptr), recv_pipeline(nullptr),
          appsrc(nullptr), appsink(nullptr),
          send_bus(nullptr), recv_bus(nullptr), loop(nullptr) {}

    ~TextServer()
    {
        running = false;
        if (logFile.is_open())
        {
            writeLog("INFO", "Server shutting down");
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

    bool initLogging()
    {
        gst_init(nullptr, nullptr);

        logFile.open(logFilename, std::ios::app);
        if (!logFile.is_open())
        {
            std::cerr << "Warning: Could not open log file: " << logFilename << std::endl;
        }

        writeLog("INFO", "Server Started");
        writeLog("INFO", "Log file: " + logFilename);
        writeLog("INFO", "Command file: " + commandFilename);
        writeLog("INFO", "Host: " + host);
        writeLog("INFO", "Command file full path: " + std::filesystem::absolute(commandFilename).string());

        return true;
    }

    bool initSend()
    {
        std::string send_pipeline_str =
            "appsrc name=src emit-signals=false is-live=true "
            "format=time caps=application/x-text ! "
            "tcpserversink host=" +
            host + " port=" + std::to_string(sendPort) +
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

        send_bus = gst_pipeline_get_bus(GST_PIPELINE(send_pipeline));
        gst_bus_add_watch(send_bus, send_bus_call, this);

        writeLog("INFO", "Send pipeline initialized on port " + std::to_string(sendPort));
        return true;
    }

    bool initReceive()
    {
        std::string recv_pipeline_str =
            "tcpserversrc host=" + host + " port=" + std::to_string(receivePort) + " ! " +
            "appsink name=sink emit-signals=true sync=false";

        GError *error = nullptr;
        recv_pipeline = gst_parse_launch(recv_pipeline_str.c_str(), &error);

        if (error)
        {
            std::string errorMsg = std::string("Error creating receive pipeline: ") + error->message;
            writeLog("ERROR", errorMsg);
            g_error_free(error);
            return false;
        }

        appsink = gst_bin_get_by_name(GST_BIN(recv_pipeline), "sink");
        if (!appsink)
        {
            writeLog("ERROR", "Failed to get appsink");
            return false;
        }

        GstAppSinkCallbacks callbacks = {nullptr, nullptr, on_new_sample};
        gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, this, nullptr);

        recv_bus = gst_pipeline_get_bus(GST_PIPELINE(recv_pipeline));
        gst_bus_add_watch(recv_bus, recv_bus_call, this);

        writeLog("INFO", "Receive pipeline initialized on port " + std::to_string(receivePort));
        return true;
    }

    bool sendText(const std::string &text)
    {
        if (!appsrc)
        {
            writeLog("ERROR", "appsrc not initialized");
            return false;
        }

        if (!clientConnected.load())
        {
            writeLog("WARN", "No client connected! Message may not be delivered.");
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

        writeLog("SEND", "To client: " + text);
        return true;
    }

    void run()
    {
        gst_element_set_state(send_pipeline, GST_STATE_PLAYING);
        gst_element_set_state(recv_pipeline, GST_STATE_PLAYING);

        writeLog("INFO", "Server started and listening");
        writeLog("INFO", "Send port (server->client): " + std::to_string(sendPort));
        writeLog("INFO", "Receive port (client->server): " + std::to_string(receivePort));
        writeLog("INFO", "Host: " + host);
        writeLog("INFO", "Waiting for client connection");

        loop = g_main_loop_new(nullptr, FALSE);
        g_main_loop_run(loop);
    }

    void stop()
    {
        if (loop && g_main_loop_is_running(loop))
        {
            g_main_loop_quit(loop);
        }
    }

    bool isClientConnected() const
    {
        return clientConnected.load();
    }
};

int main(int argc, char *argv[])
{
    std::string host = "0.0.0.0";
    int sendPort = 5000;
    int receivePort = 5001;
    std::string logFile = "server.log";
    std::string commandFilename = "command.txt";

    if (argc > 1)
        host = argv[1];
    if (argc > 2)
        sendPort = std::stoi(argv[2]);
    if (argc > 3)
        receivePort = std::stoi(argv[3]);
    if (argc > 4)
        logFile = argv[4];
    if (argc > 5)
        commandFilename = argv[5];

    std::cout << "Starting TextServer..." << std::endl;
    std::cout << "Host: " << host << std::endl;
    std::cout << "Send Port: " << sendPort << std::endl;
    std::cout << "Receive Port: " << receivePort << std::endl;

    TextServer server(host, sendPort, receivePort, logFile, commandFilename);

    if (!server.initLogging())
    {
        std::cerr << "Error initializing logging" << std::endl;
        return -1;
    }

    if (!server.initSend())
    {
        std::cerr << "Error initializing send pipeline" << std::endl;
        return -1;
    }

    if (!server.initReceive())
    {
        std::cerr << "Error initializing receive pipeline" << std::endl;
        return -1;
    }

    std::cout << "Server is running!" << std::endl;
    std::cout << "Waiting for client connections..." << std::endl;

    std::thread serverThread([&server]()
                             { server.run(); });

    serverThread.join();

    server.stop();

    std::cout << "Server stopped" << std::endl;
    return 0;
}