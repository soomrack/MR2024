#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <csignal>

std::string generate_filename() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    
    std::stringstream ss;
    ss << "recorded_video_"
       << std::setfill('0') << std::setw(2) << local_time->tm_hour << "_"
       << std::setfill('0') << std::setw(2) << local_time->tm_min << "_"
       << std::setfill('0') << std::setw(2) << local_time->tm_sec
       << ".mp4";
    
    return ss.str();
}

int main() {
    std::cout << "Starting GStreamer pipeline..." << std::endl;
    
    std::string filename = generate_filename();
    std::cout << "Recording to file: " << filename << std::endl;
    
    // Создаем команду для GStreamer
    std::string cmd = "gst-launch-1.0 -e "
                      "v4l2src device=/dev/video0 ! "
                      "\"image/jpeg,width=640,height=480,framerate=30/1\" ! "
                      "tee name=t "
                      "t. ! queue ! rtpjpegpay ! "
                      "udpsink host=10.133.231.183 port=5600 sync=false "
                      "t. ! queue ! jpegdec ! videoconvert ! "
                      "x264enc speed-preset=ultrafast tune=zerolatency ! "
                      "h264parse ! mp4mux ! "
                      "filesink location=" + filename;
    
    std::cout << "Running command:\n" << cmd << "\n" << std::endl;
    
    // Просто запускаем через system
    int result = system(cmd.c_str());
    
    std::cout << "\nGStreamer finished with code: " << result << std::endl;
    std::cout << "Video saved to: " << filename << std::endl;
    std::cout << "You can now rewind and play the video." << std::endl;
    
    return 0;
}
