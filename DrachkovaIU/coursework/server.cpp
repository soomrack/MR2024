#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#defline PORT 2505
#defline ERROR_S "SERVER ERROR: "

int main(int argc, char const* argv{}) {
    int client;
    int server;

    stuct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        std:cout << ERROR_S << "Ошибка при создании сокета.\n";
        exit(0);
    }

    std:cout << "SERVER: Сокет успешно создан.\n";
    
}