#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>

#define MaxMessageLength 256              //Максимальная длина строки сообщения (для более удобного разделения текста)
#define IsDebug true                      //Для автоматического подключения к серверу прри использовании кода на локальном компьютере

SOCKET Connection = INVALID_SOCKET;        //Подключение к сокету 

std::vector<std::string> Split(std::string StringToSplit, std::string SplitterString)
{
    std::vector<std::string> ReturnVector;
    int i = 0;
    std::string SplittedString = "";
    while (i < StringToSplit.size())
	{
		if (StringToSplit[i] == SplitterString[0])
		{
			bool IsSplitter = true;
			for (int j = 1; j < SplitterString.size(); j++)
			{
				if (StringToSplit[i + j] != SplitterString[j])
				{
					IsSplitter = false;
					break;
				}
			}
			if (IsSplitter)
			{
				ReturnVector.push_back(SplittedString);
				SplittedString = "";
				i += SplitterString.size();
				continue;
			}
		}
		SplittedString += StringToSplit[i];
		i++;
	}
	ReturnVector.push_back(SplittedString);
	return ReturnVector;
}

void ClientHandler()               //Функция клиентской обработки
{
	char msg[256];
	int ConnectionStatus;
	while (true)
	{
		ConnectionStatus = recv(Connection, msg, MaxMessageLength, NULL);   //Отвечает за то, что приходит.

		if (ConnectionStatus <= 0)                                          //Отключение сервера по каким-либо причинам.
		{
			std::cout << "Server disconnected" << std::endl;
			closesocket(Connection);
			Connection = INVALID_SOCKET;
			break;
		}

		std::cout << msg << std::endl;                                       //Вывод на экран ответа сервера.
	}
}

bool ConnectToServer(std::string ServerAddress, int Port)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		std::cerr << "Winsock init fail!" << std::endl;
		return false;
	}

	SOCKADDR_IN addr;                                          //Создание адреса
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ServerAddress.c_str());
	addr.sin_port = htons(Port);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);           //Сокет обеспечивает соединение с сервером

	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		std::cout << "Error: failed connect to server." << std::endl;
		return false;
	}

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);          //В одном потоке нельзя ждать сообщения и ввода с клавиатуры. Итого у клиента 2 потока.
	return true;
}

int main()
{
	std::string Message;

	std::cout << "Use command help to get information" << std::endl;

	if (IsDebug)
		ConnectToServer("127.0.0.1", 51111);         //Локальны адрес

	while (true)
	{
		getline(std::cin, Message);
		
		if (Message.size() >= MaxMessageLength)
		{
			std::cerr << "Too long message" << std::endl;
			continue;
		}

		std::vector<std::string> MessageVector = Split(Message, " ");                //Создание вектора строк, являющегося желаемым сообщением

		if (MessageVector[0] == "help")
		{
			std::cout << "Use command connect to connect server" << std::endl;
			std::cout << "Use command disconnect to disconnect server" << std::endl;
			std::cout << "Use command create to create room" << std::endl;
			std::cout << "Use command remove to remove room" << std::endl;
			std::cout << "Use command open to open room" << std::endl;
			std::cout << "Use command !exit to exit room" << std::endl;
			std::cout << "Use command ls to show all rooms" << std::endl;
			continue;
		}
		
		if (MessageVector[0] == "connect" && MessageVector.size() >= 3 )             //Для подключения к серверу необходмо помимо команды указать IP адрес и порт.
		{
			if (Connection == INVALID_SOCKET)                                        //Пользователь может быть подключен только к одному серверу единовременно. Здесь выполняется проверка.
			{
				// Address: "127.0.0.1:1111"
				ConnectToServer(MessageVector[1], atoi(MessageVector[2].c_str()));
			}
			else
				std::cout << "You alredy connected to server" << std::endl;
			continue;
		}
		if (MessageVector[0] == "connect" && MessageVector.size() < 3)
		{
			std::cout << "Wrong command. You have to specify server ip address and port" << std::endl;
			std::cout << "Command usage: connect 127.0.0.1 1111" << std::endl;
			continue;
		}

		if (MessageVector[0] == "disconnect")
		{
			closesocket(Connection);
			Connection = INVALID_SOCKET;
			continue;
		}

		send(Connection, Message.c_str(), MaxMessageLength, NULL);                  //Отправка сообщения на сервер
		Sleep(10);
	}
}
