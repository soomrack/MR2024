// video_streamer.h

#define WIN32_LEAN_AND_MEAN
// video_streamer.h
#pragma once
#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include "config.h"

class VideoStreamer {
private:
    std::atomic<bool> running;
    std::thread stream_thread;
    HANDLE gstreamer_process;

    void runStream() {
        std::cout << "Starting GStreamer pipeline..." << std::endl;

        std::string command =
            "gst-launch-1.0 udpsrc port=" + std::to_string(Config::VIDEO_PORT) +
            " ! application/x-rtp,encoding-name=JPEG,payload=26 ! "
            "rtpjpegdepay ! jpegdec ! autovideosink";

        std::string full_command =
            "cmd.exe /c \"cd /d \"" + Config::GSTREAMER_PATH + "\" && " + command + "\"";

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (CreateProcessA(NULL, (LPSTR)full_command.c_str(), NULL, NULL,
            FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            gstreamer_process = pi.hProcess;
            std::cout << "GStreamer started with PID: " << pi.dwProcessId << std::endl;

            // Мониторим процесс
            while (running) {
                DWORD exit_code;
                if (GetExitCodeProcess(pi.hProcess, &exit_code) && exit_code != STILL_ACTIVE) {
                    std::cout << "GStreamer process terminated unexpectedly!" << std::endl;
                    break;
                }
                Sleep(100);
            }

            // Завершаем процесс если еще работает
            TerminateProcess(pi.hProcess, 0);
            WaitForSingleObject(pi.hProcess, 5000);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else {
            std::cerr << "Failed to start GStreamer!" << std::endl;
            std::cerr << "Command: " << full_command << std::endl;
            std::cerr << "Error: " << GetLastError() << std::endl;
        }

        std::cout << "Video stream stopped" << std::endl;
    }

public:
    VideoStreamer() : running(false), gstreamer_process(NULL) {}

    ~VideoStreamer() {
        stop();
    }

    void initialize() {
        std::cout << "=== Video Streamer Initialized ===" << std::endl;
        std::cout << "Stream port: " << Config::VIDEO_PORT << std::endl;
        std::cout << "GStreamer path: " << Config::GSTREAMER_PATH << std::endl;
        std::cout << "==================================" << std::endl;
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

        if (gstreamer_process) {
            TerminateProcess(gstreamer_process, 0);
            gstreamer_process = NULL;
        }

        if (stream_thread.joinable()) {
            stream_thread.join();
        }

        std::cout << "Video streamer stopped successfully" << std::endl;
    }

    bool isRunning() const {
        return running;
    }

    void waitForExit() {
        if (stream_thread.joinable()) {
            stream_thread.join();
        }
    }
};

#endif // VIDEO_STREAMER_H