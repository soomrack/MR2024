#pragma once
#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <sstream>
#include <ctime>
#include "config.h"

class VideoStreamer {
private:
    std::atomic<bool> running;
    std::thread stream_thread;
    PROCESS_INFORMATION pi;

    std::string generateFilename() {
        std::time_t t = std::time(nullptr);
        std::tm tm;
        localtime_s(&tm, &t);

        std::ostringstream oss;
        oss << Config::RECORDING_PATH << "\\recorded_video_"
            << (tm.tm_year + 1900) << "-"
            << (tm.tm_mon + 1) << "-"
            << tm.tm_mday << "_"
            << tm.tm_hour << "-"
            << tm.tm_min << "-"
            << tm.tm_sec
            << ".mkv";
        return oss.str();
    }

    void runStream() {
        std::cout << "Starting GStreamer UDP H264 stream..." << std::endl;

        std::string output_file = generateFilename();

        // Pipeline для живого UDP-H264 потока с Pi
        std::string pipeline =
            "gst-launch-1.0 "
            "udpsrc port=" + std::to_string(Config::VIDEO_PORT) + " do-timestamp=true "
            "! application/x-rtp,encoding-name=JPEG,payload=26 "
            "! rtpjpegdepay "
            "! jpegdec "
            "! videoconvert "
            "! tee name=t "
            "t. ! queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 "
            "! autovideosink "
            "t. ! queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 "
            "! x264enc bitrate=1000 speed-preset=ultrafast tune=zerolatency key-int-max=30 "
            "! h264parse "
            "! matroskamux streamable=true "
            "! filesink location=\"" + output_file + "\"";


        std::string full_command = "cmd.exe /c \"cd /d \"" + Config::GSTREAMER_PATH + "\" && " + pipeline + "\"";

        STARTUPINFOA si = { sizeof(si) };
        ZeroMemory(&pi, sizeof(pi));

        if (CreateProcessA(
            NULL,
            (LPSTR)full_command.c_str(),
            NULL,
            NULL,
            FALSE,
            CREATE_NO_WINDOW,
            NULL,
            NULL,
            &si,
            &pi)) {

            std::cout << "GStreamer started (PID: " << pi.dwProcessId << ")" << std::endl;
            std::cout << "Recording to: " << output_file << std::endl;

            while (running) {
                DWORD exit_code;
                if (GetExitCodeProcess(pi.hProcess, &exit_code) && exit_code != STILL_ACTIVE) {
                    std::cerr << "GStreamer process exited unexpectedly!" << std::endl;
                    break;
                }
                Sleep(100);
            }

            // Остановка процесса
            TerminateProcess(pi.hProcess, 0);
            WaitForSingleObject(pi.hProcess, 5000);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else {
            std::cerr << "Failed to start GStreamer!" << std::endl;
            std::cerr << "Error code: " << GetLastError() << std::endl;
        }

        std::cout << "Video stream stopped" << std::endl;
    }

public:
    VideoStreamer() : running(false) {
        ZeroMemory(&pi, sizeof(pi));
    }

    ~VideoStreamer() {
        stop();
    }

    void initialize() {
        std::cout << "=== Video Streamer Initialized ===" << std::endl;
        std::cout << "Stream port: " << Config::VIDEO_PORT << std::endl;
        std::cout << "GStreamer path: " << Config::GSTREAMER_PATH << std::endl;
        std::cout << "Recording path: " << Config::RECORDING_PATH << std::endl;
        std::cout << "=================================" << std::endl;
    }

    void start() {
        if (running) {
            std::cout << "Video stream already running!" << std::endl;
            return;
        }

        running = true;
        stream_thread = std::thread(&VideoStreamer::runStream, this);
    }

    void stop() {
        running = false;

        if (pi.hProcess) {
            TerminateProcess(pi.hProcess, 0);
            WaitForSingleObject(pi.hProcess, 5000);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            ZeroMemory(&pi, sizeof(pi));
        }

        if (stream_thread.joinable()) {
            stream_thread.join();
        }

        std::cout << "Video streamer stopped successfully" << std::endl;
    }

    bool isRunning() const {
        return running;
    }
};

#endif // VIDEO_STREAMER_H