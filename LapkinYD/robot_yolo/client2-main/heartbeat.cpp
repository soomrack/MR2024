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

#define SERVER_IP "192.168.31.149"
#define HEARTBEAT_PORT 5003
#define HEARTBEAT_INTERVAL 2
#define CONNECTION_TIMEOUT 6
#define LOG_FILE "lost.json"

static volatile gint keep_running = 1;
static volatile gint server_connected = 0;
static time_t last_heartbeat_time = 0;
static int server_socket = -1;
static pthread_t heartbeat_thread;
static gchar *server_ip = NULL;

void write_lost_json(const char *role, int error_code)
{
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
    g_print("\nЗавершение работы\n", sig);
    keep_running = 0;
    server_connected = 0;

    if (server_socket >= 0)
    {
        const char *msg = "DISCONNECT";
        send(server_socket, msg, (int)strlen(msg), 0);
        CLOSE_SOCKET(server_socket);
        server_socket = -1;
    }
}

void *heartbeat_monitor(void *arg)
{
    char buffer[64];
    int timeout_count = 0;
    struct sockaddr_in serv_addr;

    g_print("Соединение мониторится\n");

#ifdef _WIN32
    SOCKET sock;
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        g_print("Ошибка WSAStartup: %d\n", WSAGetLastError());
        return NULL;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
#else
    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
#endif
    {
        g_print("Ошибка создания сокета: %d\n", GET_ERROR);
        return NULL;
    }

    server_socket = (int)sock;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(HEARTBEAT_PORT);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
    {
        g_print("Неверный IP адрес сервера\n");
        CLOSE_SOCKET(server_socket);
        return NULL;
    }

    g_print("Подключение к серверу %s:%d\n", server_ip, HEARTBEAT_PORT);

#ifdef _WIN32
    if (connect(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
#else
    if (connect(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
#endif
    {
        g_print("\n===========================================\n");
        g_print("ОШИБКА ПРИ ПОДКЛЮЧЕНИИ: %d\n", GET_ERROR);
        g_print("===========================================\n\n");
        CLOSE_SOCKET(server_socket);
        server_socket = -1;
        server_connected = 0;
#ifdef _WIN32
        WSACleanup();
#endif
        return NULL;
    }

    g_print("\n===========================================\n");
    g_print("ПОДКЛЮЧЕНО К СЕРВЕРУ: %s\n", server_ip);
    g_print("===========================================\n\n");

    server_connected = 1;
    last_heartbeat_time = time(NULL);

    SET_NONBLOCK(server_socket);

    while (keep_running && server_connected)
    {
        SLEEP_SEC(HEARTBEAT_INTERVAL);

        if (!server_connected)
        {
            break;
        }

        snprintf(buffer, sizeof(buffer), "HEARTBEAT:%ld", (long)time(NULL));

        int sent = send(server_socket, buffer, (int)strlen(buffer), 0);

        if (sent < 0)
        {
#ifdef _WIN32
            int err = WSAGetLastError();
            if (err == WSAECONNRESET)
            {
                g_print("\n===========================================\n");
                g_print("ОШИБКА ОТПРАВКИ: 10054 (Connection reset)\n");
                g_print("===========================================\n\n");

                write_lost_json("client", 10054);
            }
            else
            {
                g_print("\n===========================================\n");
                g_print("ОШИБКА ОТПРАВКИ: %d\n", err);
                g_print("===========================================\n\n");

                write_lost_json("client", err);
            }
#else
            if (errno == ECONNRESET)
            {
                g_print("\n===========================================\n");
                g_print("ОШИБКА ОТПРАВКИ: ECONNRESET\n");
                g_print("===========================================\n\n");

                write_lost_json("client", ECONNRESET);
            }
            else
            {
                g_print("\n===========================================\n");
                g_print("ОШИБКА ОТПРАВКИ: %d\n", errno);
                g_print("===========================================\n\n");

                write_lost_json("client", errno);
            }
#endif
            server_connected = 0;
            break;
        }

        g_print("Heartbeat отправлен\n");

        char recv_buffer[1024] = {0};
        int valread = recv(server_socket, recv_buffer, sizeof(recv_buffer) - 1, 0);

        if (valread > 0)
        {
            last_heartbeat_time = time(NULL);
            g_print("Heartbeat получен от сервера\n");
            timeout_count = 0;
        }
        else if (valread == 0)
        {
            g_print("\n===========================================\n");
            g_print("Сервер закрыл соединение\n");
            g_print("===========================================\n\n");

            // Запись в JSON
            write_lost_json("client", 0);

            server_connected = 0;
            break;
#ifdef _WIN32
        }
        else if (WSAGetLastError() == WSAECONNRESET)
        {
            g_print("\n===========================================\n");
            g_print("ОШИБКА ЧТЕНИЯ: 10054 (Connection reset)\n");
            g_print("===========================================\n\n");

            write_lost_json("client", 10054);

            server_connected = 0;
            break;
        }
        else if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
#else
        }
        else if (errno == ECONNRESET)
        {
            g_print("\n===========================================\n");
            g_print("ОШИБКА ЧТЕНИЯ: ECONNRESET\n");
            g_print("===========================================\n\n");

            write_lost_json("client", ECONNRESET);

            server_connected = 0;
            break;
        }
        else if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
#endif
            g_print("\n===========================================\n");
            g_print("ОШИБКА ЧТЕНИЯ: %d\n", GET_ERROR);
            g_print("===========================================\n\n");

            write_lost_json("client", GET_ERROR);

            server_connected = 0;
            break;
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
                g_print("\n===========================================\n");
                g_print("Потеря соединения с сервером\n");
                g_print("===========================================\n");
                g_print("Последняя активность: %ld сек назад\n\n", (long)elapsed);

                write_lost_json("client", 0);

                server_connected = 0;
                break;
            }
        }
        else
        {
            timeout_count = 0;
        }
    }

    if (server_socket >= 0)
    {
        CLOSE_SOCKET(server_socket);
        server_socket = -1;
    }

#ifdef _WIN32
    WSACleanup();
#endif

    g_print("Остановил мониторинг\n");
    return NULL;
}

void print_usage(const char *prog_name)
{
    g_print("Использование: %s <server_ip>\n", prog_name);
}

int main(int argc, char *argv[])
{
    pthread_t heartbeat_thread;

    server_ip = (gchar *)SERVER_IP;

    gst_init(&argc, &argv);

    g_print("Сервер IP: %s\n", server_ip);
    g_print("Порт: %d\n", HEARTBEAT_PORT);
    g_print("Интервал heartbeat: %d сек\n", HEARTBEAT_INTERVAL);
    g_print("Таймаут соединения: %d сек\n", CONNECTION_TIMEOUT);
    g_print("Лог файл: %s\n\n", LOG_FILE);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    pthread_create(&heartbeat_thread, NULL, heartbeat_monitor, NULL);

    while (keep_running)
    {
        SLEEP_SEC(1);
    }

    g_print("Очистка ресурсов\n");

    pthread_join(heartbeat_thread, NULL);

    g_print("Клиент остановлен\n");

    gst_deinit();

    return 0;
}