#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int main(int argc, char** argv) {



	//error enum
	enum errors
	{
		ERROR_WSASTARTUP = 1,
		ERROR_GETADDRINFO = 2,
		ERROR_CREATESOCKET = 3,
		ERROR_CONNECTSOCKET = 4,
		ERROR_SENDFAILED = 5,
		ERROR_SHUTDOWNFAILED = 6
	};



	WSADATA wsadata;

	int iResult;

	//initialize winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed, error: " << iResult << std::endl;
		return ERROR_WSASTARTUP;
	}

#pragma region Client

	//creating addrinfo struct
	struct addrinfo* serverResult = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &serverResult);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed, error:" << iResult << std::endl;
		WSACleanup();
		return ERROR_GETADDRINFO;
	}

	SOCKET connectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by the call to getaddrinfo
	ptr = serverResult;

	// Create a SOCKET for connecting to server
	connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (connectSocket == INVALID_SOCKET)
	{
		std::cout << "Error at socket: " << WSAGetLastError() << std::endl;
		freeaddrinfo(serverResult);
		WSACleanup();
		return ERROR_CREATESOCKET;
	}


	//Connect to a server
	iResult = connect(connectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	//todo: try next addresses returned by getaddrinfo if the connect call above fails

	freeaddrinfo(serverResult);

	if (connectSocket == INVALID_SOCKET)
	{
		std::cout << "Unable to connect to server!" << std::endl;
		WSACleanup();
		return ERROR_CONNECTSOCKET;
	}

	//Sending and recieving data
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];

	const char* sendbuf = "this is a test";

	//send an initial buffer
	iResult = send(connectSocket, sendbuf, static_cast<int>(strlen(sendbuf)), 0);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Send failed, error: " << WSAGetLastError() << std::endl;
		closesocket(connectSocket);
		WSACleanup();
		return ERROR_SENDFAILED;
	}

	std::cout << "Bytes sent: " << iResult;

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Shutdown failed, error: " << WSAGetLastError() << std::endl;
		closesocket(connectSocket);
		WSACleanup();
		return ERROR_SHUTDOWNFAILED;
	}

	// Receive data until the server closes the connection
	do
	{
		iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			std::cout << "Bytes received: " << iResult << std::endl;
		}
		else if (iResult == 0)
		{
			std::cout << "Connection closed" << std::endl;
		}
		else
		{
			std::cout << "Receive failed, error: " << WSAGetLastError() << std::endl;
		}
	} while (iResult > 0);


	//disconnecting the client
	closesocket(connectSocket);
	WSACleanup();

#pragma endregion

#pragma region Server

#pragma endregion

	return 0;
}