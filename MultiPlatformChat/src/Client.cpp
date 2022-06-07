#include "Client.h"

#include <string>

namespace Engine
{

	void Client::setDefault() {
		inet_pton(AF_INET, this->m_serverName.c_str(), &this->m_socketInfo.sin_addr);
	}

	void Client::showMessage(const std::string& t_message) {
		if (this->m_onMessage != nullptr) {
			this->m_onMessage(t_message);
		}
	}

	void Client::_connectServer() {
		this->createSocket();

		if (connect(this->m_socketFd, reinterpret_cast<sockaddr*> (&this->m_socketInfo), this->m_addressLen) == -1) {
			std::cout << "Could not connect to server!" << std::endl;
			std::getchar();
			std::getchar();
			throw std::runtime_error("could not connect to server");
		}
	}

	void Client::receiveMessage() {
		this->m_thread = std::move(std::thread([=] {
			int len;
			char message_buffer[MESSAGE_SIZE];

			while ((len = recv(this->m_socketFd, message_buffer, this->getMessageSize(), 0)) > 0) {
				message_buffer[len] = '\0';
				showMessage(message_buffer);
				std::memset(message_buffer, '\0', sizeof(message_buffer));
			}
			this->m_state = STATE::CLOSED;
			}));
	}

	void Client::writeMessage() {
		std::string message = "";
		while (this->m_state == STATE::OPEN) {
			std::getline(std::cin, message);
			this->sendMessage(this->m_socketFd, message);
		}
	}

	Client::Client() :
		NetworkUnit() {
		setDefault();
	}

	Client::Client(const int& port) :
		NetworkUnit(port) {
		setDefault();
	}

	Client::Client(const std::string& serverName, const int& port) :
		NetworkUnit(port), m_serverName(serverName) {
		setDefault();
	}

	void Client::setOnMessage(const std::function<void(const std::string&)>& function) {
		m_onMessage = function;
	}

	void Client::connectServer() {
		this->_connectServer();
		showMessage("Connection successfully!");
		this->receiveMessage();
		this->writeMessage();
	}

	void Client::closeConnection() {
		this->closeSocket();
		if (this->m_thread.joinable()) {
			this->m_thread.join();
		}
	}

	Client::~Client() {
		this->closeConnection();
	}

}
