#include "Client.h"

const char SERVER_IP[] = "127.0.0.1"; // Enter IPv4 address of Server


	int Client::Inet_pton(int af, const char* src, void* dst) {
		struct sockaddr_storage ss;
		int size = sizeof(ss);
		wchar_t src_copy[INET6_ADDRSTRLEN + 1];
		size_t convertedChars = 0;

		ZeroMemory(&ss, sizeof(ss));
		/* stupid non-const API */
		if (mbstowcs_s(&convertedChars, src_copy, src, INET6_ADDRSTRLEN + 1) != 0) {
			std::cout << "Error converting string to wide character." << std::endl;
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

	// WinSock initialization
	int Client::Start() {
		erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

		if (erStat != 0) {
			std::cout << "Error WinSock version initializaion #";
			std::cout << WSAGetLastError();
			return 1;

		}
		else
		{
			std::cout << "WinSock initialization is OK" << std::endl;
		}

		// Socket initialization
		ClientSock = socket(AF_INET, SOCK_STREAM, 0);
		if (ClientSock == INVALID_SOCKET) {
			std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
			closesocket(ClientSock);
			WSACleanup();

		}
		else
			std::cout << "Client socket initialization is OK" << std::endl;


		// Establishing a connection to Server
		sockaddr_in servInfo;

		ZeroMemory(&servInfo, sizeof(servInfo));

		servInfo.sin_family = AF_INET;
		Inet_pton(AF_INET, SERVER_IP, &servInfo.sin_addr);
		servInfo.sin_port = htons(SERVER_PORT_NUM);

		erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

		if (erStat != 0) {
			std::cout << "Connection to Server is FAILED. Error # " <<
				WSAGetLastError() << std::endl;
			closesocket(ClientSock);
			WSACleanup();
			return 1;

		}
		else
			std::cout << "Connection established SUCCESSFULLY. " << std::endl;
		Chat();
		return 0;
	}

	// Exchange text data between Server and Client.
	// Disconnection if a Client send "xxx"
	int Client::Chat()
	{
		std::cout << "Ready to send a message to Server" << std::endl;

		std::vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);
		// The size of sending / receiving packet in bytes
		short packet_size = 0;

		while (true) {
			std::cout << "Your (Client) message to Server: ";
			fgets(clientBuff.data(), clientBuff.size(), stdin);
			// Check whether client like to stop chatting
			if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
				shutdown(ClientSock, SD_BOTH);
				End();
				return 0;

			}

			packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);

			if (packet_size == SOCKET_ERROR) {
				std::cout << "Can't send message to Server. Error # " <<
					WSAGetLastError() << std::endl;
				End();
				return 1;

			}

			packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);

			if (packet_size == SOCKET_ERROR) {
				std::cout << "Can't receive message from Server. Error # " <<
					WSAGetLastError() << std::endl;
				End();
				return 1;

			}
			else
				std::cout << "Server message: " << servBuff.data() << std::endl;

		}
		End();
		return 0;
	}
	
	int Client::End()
	{
		closesocket(ClientSock);
		WSACleanup();
		return 0;
	}
