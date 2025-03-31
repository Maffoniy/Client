#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class Client
{
private:
	SOCKET ClientSock;
	WSADATA wsData;
	//info between server and client
	//	Key variables for all program
	//	For checking errors in sockets functions
	static const short SERVER_PORT_NUM = 5544; // Enter Listening port on Server side
	static const short BUFF_SIZE = 1024; // Maximum size of buffer for exchange
	int erStat;
public:
	int Inet_pton(int af, const char* src, void* dst);
	int Start();
	int Chat();
	int End();
};

