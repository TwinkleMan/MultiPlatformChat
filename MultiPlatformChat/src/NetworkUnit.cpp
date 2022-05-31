#include "NetworkUnit.h"

namespace Engine
{
	void NetworkUnit::setSocketInfo()
	{
		this->m_addressLen = sizeof(this->m_socketInfo);
		std::memset(&this->m_socketInfo, 0, this->m_addressLen);
		this->m_socketInfo.sin_family = AF_INET;
		this->m_socketInfo.sin_port = htons(this->m_port);
	}

	NetworkUnit::NetworkUnit()
	{
		setSocketInfo();
	}

	NetworkUnit::NetworkUnit(const int& port) : m_port(port)
	{
		setSocketInfo();
	}

	int NetworkUnit::getPort() const
	{
		return m_port;
	}

	void NetworkUnit::setPort(const int& port)
	{
		m_port = port;
		setSocketInfo();
	}

	int NetworkUnit::getSocketFd() const {
		return m_socketFd;
	}

	void NetworkUnit::setSocketFd(const int& socketFd) {
		m_socketFd = socketFd;
	}

	sockaddr_in NetworkUnit::getSocketInfo() const {
		return m_socketInfo;
	}

	void NetworkUnit::setSocketInfo(const sockaddr_in& socketInfo) {
		m_socketInfo = socketInfo;
	}

	int NetworkUnit::getMessageSize() const {
		return m_messageSize;
	}

	STATE NetworkUnit::getState() const {
		return m_state;
	}

	void NetworkUnit::setState(const STATE& state) {
		m_state = state;
	}

	void NetworkUnit::setMessageSize(const int& messageSize) {
		m_messageSize = messageSize;
	}

	WORKMODE NetworkUnit::getWorkmode() const
	{
		return m_workMode;
	}

	 void NetworkUnit::setWorkmode(const WORKMODE& workmode)
	 {
		 m_workMode = workmode;
	 }



	 void NetworkUnit::createSocket() {

		 #ifdef _WIN32
		 if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
			 throw std::runtime_error("WSAStartup() failed");
		 }
		 #endif

		 this->m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
		 if (this->m_socketFd == -1) {
			 throw std::runtime_error("could not create socket");
		 }
		 this->m_state = STATE::OPEN;
	 }

	 void NetworkUnit::sendMessage(const int& socketFd, const std::string& message) {
		 send(socketFd, message.c_str(), message.size(), 0);
	 }

	 void NetworkUnit::closeSocket() {
		 if (this->m_state == STATE::CLOSED) {
			 return;
		 }
		 #ifdef _WIN32
		 if (::closesocket(this->m_socketFd) < 0) {
			 throw std::runtime_error("Could not close socket");
		 }
		 ::WSACleanup();
		 #else
		 if (close(this->m_socketFd) < 0) {
			 throw std::runtime_error("Could not close socket");
		 }
		 #endif
		 this->m_state = STATE::CLOSED;
	 }

	 NetworkUnit::~NetworkUnit() {
		 closeSocket();
	 }

}
