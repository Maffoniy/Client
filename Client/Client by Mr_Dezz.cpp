
// Client part for Server-Client chat. Developed by Mr_Dezz
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int Inet_pton(int af, const char* src, void* dst) {
	struct sockaddr_storage ss;
	int size = sizeof(ss);
	wchar_t src_copy[INET6_ADDRSTRLEN + 1];
	size_t convertedChars = 0;

	ZeroMemory(&ss, sizeof(ss));
	/* stupid non-const API */
	if (mbstowcs_s(&convertedChars, src_copy, src, INET6_ADDRSTRLEN + 1) != 0) {
        cout << "Error converting string to wide character." << endl;
        return 0;
    }

	if (WSAStringToAddressW(src_copy, af, NULL, (struct sockaddr*)&ss, &size) == 0) {
		switch (af) {
		case AF_INET:
			*(struct in_addr*)dst = ((struct sockaddr_in*)&ss)->sin_addr;
			return 1;
		case AF_INET6:
			*(struct in6_addr*)dst = ((struct sockaddr_in6*)&ss)->sin6_addr;
			return 1;

		}

	}
	return 0;

}

int main(void) {
	// Key constants
	const char SERVER_IP[] = "127.0.0.1"; // Enter IPv4 address of Server
	const short SERVER_PORT_NUM = 5544; // Enter Listening port on Server side
	const short BUFF_SIZE = 1024; // Maximum size of buffer for exchange
	//info between server and client
		// Key variables for all program
		// For checking errors in sockets functions
	int erStat;
	// IP in string format to numeric format for socket functions.
		// Data is in "ip_to_num"

	// WinSock initialization
	WSADATA wsData;
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;

	}
	else
		cout << "WinSock initialization is OK" << endl;

	// Socket initialization
	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();

	}
	else
		cout << "Client socket initialization is OK" << endl;


	// Establishing a connection to Server
	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	Inet_pton(AF_INET, SERVER_IP, &servInfo.sin_addr);
	servInfo.sin_port = htons(SERVER_PORT_NUM);
	
	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Connection to Server is FAILED. Error # " <<
			WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;

	}
	else
		cout << "Connection established SUCCESSFULLY. " <<
		"Ready to send a message to Server" << endl;


	// Exchange text data between Server and Client.
	// Disconnection if a Client send "xxx"

	vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);
	// The size of sending / receiving packet in bytes
	short packet_size = 0;

	while (true) {
		cout << "Your (Client) message to Server: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);
		// Check whether client like to stop chatting
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientSock, SD_BOTH);
			closesocket(ClientSock);
			WSACleanup();
			return 0;

		}

		packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message to Server. Error # " <<
				WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			return 1;

		}

		packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't receive message from Server. Error # " <<
				WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			return 1;

		}
		else
			cout << "Server message: " << servBuff.data() << endl;

	}

	closesocket(ClientSock);
	WSACleanup();
	return 0;

}