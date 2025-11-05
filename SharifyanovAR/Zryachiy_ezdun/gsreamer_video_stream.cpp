#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    std::cout << "Starting GStreamer pipeline..." << std::endl;
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний процесс - запускаем GStreamer
        std::cout << "Child process: Starting GStreamer" << std::endl;
        
        execlp("gst-launch-1.0", "gst-launch-1.0", //здесь вместо 10.249.15.183 вводиим ip компа в локальной сети
               "v4l2src", "device=/dev/video0", "!",
               "image/jpeg,width=640,height=480,framerate=30/1", "!",
               "rtpjpegpay", "!", 
               "udpsink", "host=10.249.15.183", "port=5600", "sync=false",
               NULL);
        
        // Если выполнение дошло до этой точки - ошибка!
        std::cerr << "ERROR: Failed to execute GStreamer!" << std::endl;
        return 1;
        
    } else if (pid > 0) {
        // Родительский процесс
        std::cout << "Parent process: GStreamer started with PID: " << pid << std::endl;
        std::cout << "Press Ctrl+C to stop the stream" << std::endl;
        
        // Ждем завершения дочернего процесса
        int status;
        waitpid(pid, &status, 0);
        
        std::cout << "GStreamer finished with status: " << status << std::endl;
        
    } else {
        // Ошибка создания процесса
        std::cerr << "ERROR: Failed to create process!" << std::endl;
        return 1;
    }
    
    return 0;
}
