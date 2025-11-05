// ###Это программа для того чтобы "слушать" порт команд на малине, команды при жтом приходят с компа

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <atomic>

std::atomic<bool> running(true);

void handleCommand(char command) {
    switch(command) {
        case 'w':
            std::cout << "Команда: ВПЕРЕД" << std::endl;
            // Здесь код для движения вперед
            break;
        case 'a':
            std::cout << "Команда: ВЛЕВО" << std::endl;
            // Здесь код для движения влево
            break;
        case 's':
            std::cout << "Команда: НАЗАД" << std::endl;
            // Здесь код для движения назад
            break;
        case 'd':
            std::cout << "Команда: ВПРАВО" << std::endl;
            // Здесь код для движения вправо
            break;
        case ' ':
            std::cout << "Команда: СТОП" << std::endl;
            // Здесь код для остановки
            break;
        case 'q':
            std::cout << "Завершение работы..." << std::endl;
            running = false;
            break;
        default:
            std::cout << "Неизвестная команда: " << command << std::endl;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    // Создаем socket файловый дескриптор
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Ошибка создания socket" << std::endl;
        return -1;
    }
    
    // Устанавливаем опции socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Ошибка setsockopt" << std::endl;
        return -1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Принимаем соединения с любого IP
    address.sin_port = htons(8888); // Порт 8888
    
    // Биндим socket к порту
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Ошибка bind" << std::endl;
        return -1;
    }
    
    // Начинаем слушать входящие соединения
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Ошибка listen" << std::endl;
        return -1;
    }
    
    std::cout << "Сервер запущен. Ожидание подключения на порту 8888..." << std::endl;
    
    // Принимаем входящее соединение
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Ошибка accept" << std::endl;
        return -1;
    }
    
    std::cout << "Клиент подключен!" << std::endl;
    std::cout << "Доступные команды: w, a, s, d, space, q(выход)" << std::endl;
    
    // Главный цикл приема команд
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        
        // Читаем данные от клиента
        int valread = read(new_socket, buffer, 1024);
        
        if (valread > 0) {
            // Обрабатываем каждую команду в буфере
            for (int i = 0; i < valread; i++) {
                handleCommand(buffer[i]);
            }
        } else if (valread == 0) {
            std::cout << "Клиент отключился" << std::endl;
            break;
        } else {
            std::cerr << "Ошибка чтения" << std::endl;
            break;
        }
    }
    
    // Закрываем соединения
    close(new_socket);
    close(server_fd);
    
    std::cout << "Сервер остановлен" << std::endl;
    return 0;
}
