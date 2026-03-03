#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <csignal>
#include <sys/stat.h>
#include <string>
#include <cstdlib>

std::string get_desktop_path() {
    const char* home = getenv("HOME");
    if (home == nullptr) {
        return "./";  // fallback to current directory
    }
    return std::string(home) + "/Desktop/";
}

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

bool create_directory(const std::string& path) {
    struct stat st = {0};
    if (stat(path.c_str(), &st) == -1) {
        return (mkdir(path.c_str(), 0755) == 0);
    }
    return true;  // directory already exists
}

std::string get_ip_address() {
    std::string ip;
    int choice;
    
    std::cout << "\nВыберите IP адрес для отправки видео:" << std::endl;
    std::cout << "1. Использовать дефолтный IP (10.209.227.183)" << std::endl;
    std::cout << "2. Ввести свой IP адрес" << std::endl;
    std::cout << "Ваш выбор (1 или 2): ";
    
    std::cin >> choice;
    
    if (choice == 1) {
        ip = "10.209.227.183";
        std::cout << "Используется дефолтный IP: " << ip << std::endl;
    } else if (choice == 2) {
        std::cout << "Введите IP адрес: ";
        std::cin >> ip;
        std::cout << "Используется IP: " << ip << std::endl;
    } else {
        std::cout << "Неверный выбор. Используется дефолтный IP." << std::endl;
        ip = "10.209.227.183";
    }
    
    return ip;
}

int main() {
    std::cout << "Starting GStreamer pipeline..." << std::endl;
    
    // Получаем путь к рабочему столу и создаем папку для видео
    std::string desktop_path = get_desktop_path();
    std::string video_folder = desktop_path + "recorded_videos/";
    
    if (create_directory(video_folder)) {
        std::cout << "Папка для видео: " << video_folder << std::endl;
    } else {
        std::cerr << "Не удалось создать папку для видео. Используется рабочий стол." << std::endl;
        video_folder = desktop_path;
    }
    
    // Получаем IP адрес от пользователя
    std::string ip_address = get_ip_address();
    
    // Генерируем полный путь к файлу
    std::string filename = video_folder + generate_filename();
    std::cout << "Запись в файл: " << filename << std::endl;
    
    // Создаем команду для GStreamer с выбранным IP
    std::string cmd = "gst-launch-1.0 -e "
                      "v4l2src device=/dev/video0 ! "
                      "\"image/jpeg,width=640,height=480,framerate=30/1\" ! "
                      "tee name=t "
                      "t. ! queue ! rtpjpegpay ! "
                      "udpsink host=" + ip_address + " port=5600 sync=false "
                      "t. ! queue ! jpegdec ! videoconvert ! "
                      "x264enc speed-preset=ultrafast tune=zerolatency ! "
                      "h264parse ! mp4mux ! "
                      "filesink location=\"" + filename + "\"";
    
    std::cout << "\nЗапуск команды:\n" << cmd << "\n" << std::endl;
    
    // Просто запускаем через system
    int result = system(cmd.c_str());
    
    std::cout << "\nGStreamer завершился с кодом: " << result << std::endl;
    std::cout << "Видео сохранено в: " << filename << std::endl;
    std::cout << "Вы можете перемотать и воспроизвести видео." << std::endl;
    
    return 0;
}
