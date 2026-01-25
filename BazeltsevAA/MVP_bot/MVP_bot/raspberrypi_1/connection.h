#pragma once
#include "TcpSocket.h"
#include "ConnectionInfo.h"
#include <string>

ConnectionInfo wait_op_connection(TcpSocket& server, uint16_t port, const std::string& baseDir);

