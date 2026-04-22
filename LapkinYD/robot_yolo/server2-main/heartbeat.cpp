#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define CLOSE_SOCKET closesocket
#define GET_ERROR WSAGetLastError()
#define SET_NONBLOCK(sock)                 \
    {                                      \
        u_long mode = 1;                   \
        ioctlsocket(sock, FIONBIO, &mode); \
    }
#define SLEEP_SEC(sec) Sleep((sec) * 1000)
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define CLOSE_SOCKET close
#define GET_ERROR errno
#define SET_NONBLOCK(sock)                        \
    {                                             \
        int flags = fcntl(sock, F_GETFL, 0);      \
        fcntl(sock, F_SETFL, flags | O_NONBLOCK); \
    }
#define SLEEP_SEC(sec) sleep(sec)
#define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

#define HEARTBEAT_PORT 5003
#define HEARTBEAT_INTERVAL 2
#define CONNECTION_TIMEOUT 6
#define LOG_FILE "lost.json"
#define HEARTBEAT_FILE "heartbeat.txt"

static volatile gint keep_running = 1;
static volatile gint client_connected = 0;
static time_t last_heartbeat_time = 0;
static int client_socket = -1;
static pthread_t heartbeat_thread;

static gint heartbeat_file_state = -1;
static volatile gint connection_was_established = 0;
void write_heartbeat_file(int state)
{
    if (!connection_was_established)
    {
        return;
    }

    if (state == heartbeat_file_state)
    {
        return;
    }

    FILE *fp = fopen(HEARTBEAT_FILE, "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", state);
        fclose(fp);
        heartbeat_file_state = state;

        if (state == 1)
        {
            g_print("ПОТЕРЯ СВЯЗИ\n");
        }
        else
        {
            g_print("СВЯЗЬ ЕСТЬ\n");
        }
    }
    else
    {
        g_print("Ошибка открытия %s для записи\n", HEARTBEAT_FILE);
    }
}

void write_lost_json(const char *role, int error_code)
{
    if (!connection_was_established)
    {
        return;
    }

    FILE *fp;
    char timestamp[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fp = fopen(LOG_FILE, "w");
    if (fp != NULL)
    {
        fprintf(fp, "{\n");
        fprintf(fp, "    \"message\": \"Lost the connection\",\n");
        fprintf(fp, "    \"role\": \"%s\",\n", role);
        fprintf(fp, "    \"error_code\": %d,\n", error_code);
        fprintf(fp, "    \"timestamp\": \"%s\"\n", timestamp);
        fprintf(fp, "}\n");
        fclose(fp);
        g_print("Запись в %s выполнена\n", LOG_FILE);
    }
    else
    {
        g_print("Ошибка открытия %s для записи\n", LOG_FILE);
    }
}

void signal_handler(int sig)
{
    g_print("\nПолучен сигнал %d, завершение работы\n", sig);
    keep_running = 0;
    client_connected = 0;

    if (connection_was_established)
    {
        write_heartbeat_file(1);
    }

    if (client_socket >= 0)
    {
        CLOSE_SOCKET(client_socket);
        client_socket = -1;
    }
}

void *heartbeat_monitor(void *arg)
{
    int timeout_count = 0;
    char buffer[64];

    g_print("Мониторю соединение\n");

    if (connection_was_established)
    {
        write_heartbeat_file(0);
    }

    while (keep_running)
    {
        SLEEP_SEC(HEARTBEAT_INTERVAL);

        if (!client_connected)
        {
            if (connection_was_established)
            {
                write_heartbeat_file(1);
            }
            timeout_count = 0;
            continue;
        }

        write_heartbeat_file(0);

        snprintf(buffer, sizeof(buffer), "HEARTBEAT:%ld", (long)time(NULL));

        if (client_socket >= 0)
        {
            int sent = send(client_socket, buffer, (int)strlen(buffer), 0);

            if (sent < 0)
            {
#ifdef _WIN32
                int err = WSAGetLastError();
                if (err != WSAEWOULDBLOCK)
#else
                if (errno != EAGAIN && errno != EWOULDBLOCK)
#endif
                {
                    g_print("Ошибка отправки: %d\n", GET_ERROR);
                    timeout_count++;
                }
            }
            else
            {
                g_print("Heartbeat отправлен\n");
            }
        }

        time_t current_time = time(NULL);
        time_t elapsed = current_time - last_heartbeat_time;

        if (elapsed > CONNECTION_TIMEOUT)
        {
            timeout_count++;
            g_print("Таймаут heartbeat (%d/3), прошло %ld сек\n",
                    timeout_count, (long)elapsed);

            if (timeout_count >= 3)
            {
                g_print("ПОТЕРЯ СОЕДИНЕНИЯ С КЛИЕНТОМ!\n");
                g_print("Последняя активность: %ld сек назад\n", (long)elapsed);
                g_print("\n");

                write_lost_json("server", 0);

                client_connected = 0;

                if (client_socket >= 0)
                {
                    CLOSE_SOCKET(client_socket);
                    client_socket = -1;
                }

                timeout_count = 0;
            }
        }
        else
        {
            timeout_count = 0;
        }
    }

    g_print("Монитор соединения остановлен\n");
    return NULL;
}

void *heartbeat_server(void *arg)
{
#ifdef _WIN32
    SOCKET server_fd, client_fd;
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        g_print("Ошибка WSAStartup: %d\n", WSAGetLastError());
        return NULL;
    }
#else
    int server_fd, client_fd;
#endif

    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

#ifdef _WIN32
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
#else
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
#endif
    {
        g_print("Ошибка создания сокета: %d\n", GET_ERROR);
        return NULL;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
    {
        g_print("Ошибка setsockopt: %d\n", GET_ERROR);
        CLOSE_SOCKET(server_fd);
        return NULL;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(HEARTBEAT_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        g_print("Ошибка bind: %d\n", GET_ERROR);
        CLOSE_SOCKET(server_fd);
        return NULL;
    }

    if (listen(server_fd, 3) < 0)
    {
        g_print("Ошибка listen: %d\n", GET_ERROR);
        CLOSE_SOCKET(server_fd);
        return NULL;
    }

    g_print("Сервер запущен на порту %d\n", HEARTBEAT_PORT);
    g_print("Ожидание подключения клиента\n");

    while (keep_running)
    {
#ifdef _WIN32
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET)
#else
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
#endif
        {
            if (keep_running)
            {
                g_print("Ошибка accept: %d\n", GET_ERROR);
            }
            continue;
        }

        client_socket = (int)client_fd;

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, ip_str, sizeof(ip_str));

        g_print("КЛИЕНТ ПОДКЛЮЧЕН: %s\n", ip_str);

        client_connected = 1;
        last_heartbeat_time = time(NULL);

        connection_was_established = 1;
        write_heartbeat_file(0);

        SET_NONBLOCK(client_socket);

        while (keep_running && client_connected)
        {
            memset(buffer, 0, sizeof(buffer));
            int valread = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

            if (valread > 0)
            {
                last_heartbeat_time = time(NULL);

                if (strncmp(buffer, "HEARTBEAT:", 10) == 0)
                {
                    g_print("Heartbeat получен от клиента\n");
                }
                else if (strncmp(buffer, "DISCONNECT", 10) == 0)
                {
                    g_print("Клиент инициировал отключение\n");
                    client_connected = 0;
                    break;
                }
            }
            else if (valread == 0)
            {
                g_print("Клиент закрыл соединение\n");
                client_connected = 0;
                write_lost_json("server", 0);
                write_heartbeat_file(1);
                break;
#ifdef _WIN32
            }
            else if (WSAGetLastError() == WSAECONNRESET)
            {
                g_print("ОШИБКА ЧТЕНИЯ: 10054 (Connection reset)\n");
                write_lost_json("server", 10054);
                write_heartbeat_file(1);

                client_connected = 0;
                break;
            }
            else if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
#else
            }
            else if (errno == ECONNRESET)
            {
                g_print("ОШИБКА ЧТЕНИЯ: ECONNRESET\n");
                write_lost_json("server", ECONNRESET);
                write_heartbeat_file(1);

                client_connected = 0;
                break;
            }
            else if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
#endif
                g_print("Ошибка чтения: %d\n", GET_ERROR);
                write_lost_json("server", GET_ERROR);
                write_heartbeat_file(1);

                client_connected = 0;
                break;
            }

            SLEEP_MS(100);
        }

        if (client_socket >= 0)
        {
            CLOSE_SOCKET(client_socket);
            client_socket = -1;
        }

        if (!client_connected && keep_running)
        {
            g_print("Ожидание нового подключения\n");
        }
    }

    CLOSE_SOCKET(server_fd);

#ifdef _WIN32
    WSACleanup();
#endif

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t heartbeat_srv_thread;

    gst_init(&argc, &argv);

    g_print("Порт: %d\n", HEARTBEAT_PORT);
    g_print("Интервал heartbeat: %d сек\n", HEARTBEAT_INTERVAL);
    g_print("Таймаут соединения: %d сек\n", CONNECTION_TIMEOUT);
    g_print("Лог файл: %s\n", LOG_FILE);
    g_print("Heartbeat файл: %s\n", HEARTBEAT_FILE);
    g_print("Потеря = только после установки связи\n\n");

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    pthread_create(&heartbeat_srv_thread, NULL, heartbeat_server, NULL);
    pthread_create(&heartbeat_thread, NULL, heartbeat_monitor, NULL);

    while (keep_running)
    {
        SLEEP_SEC(1);
    }

    g_print("Очистка ресурсов\n");

    pthread_join(heartbeat_thread, NULL);
    pthread_join(heartbeat_srv_thread, NULL);

    g_print("Сервер остановлен\n");

    gst_deinit();

    return 0;
}