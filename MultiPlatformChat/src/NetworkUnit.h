#pragma once

//Multiplatform enable

#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
static WSAData wsaData;
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#endif

namespace Engine
{
	enum class STATE
	{
		OPEN,
		CLOSED
	};

	enum class WORKMODE
	{
		CLIENT,
		SERVER,
		BOTH
	};

	class NetworkUnit
	{
	public:
		NetworkUnit();
		NetworkUnit(const int& port);

		// Port
		int getPort() const;
		void setPort(const int& port);

		// Socket file descriptor
		int getSocketFd() const;
		void setSocketFd(const int& socketFd);

		// Socket information
		sockaddr_in getSocketInfo() const;
		void setSocketInfo(const sockaddr_in& socketInfo);

		// Connection status
		STATE getState() const;
		void setState(const STATE& state);

		//workmode
		WORKMODE getWorkmode() const;
		void setWorkmode(const WORKMODE& workmode);

		// Message size
		int getMessageSize() const;
		void setMessageSize(const int& messageSize);

		virtual ~NetworkUnit();

	protected:
		WORKMODE m_workMode;

		//socket
		int m_socketFd;
		struct sockaddr_in m_socketInfo;
		int m_port = 5000;
		int m_addressLen = 0;
		int m_messageSize = 4096;
		STATE m_state;

	protected:
		void setSocketInfo();
		void createSocket();
		void sendMessage(const int& socketFd, const std::string& message);
		void closeSocket();
	};
}
