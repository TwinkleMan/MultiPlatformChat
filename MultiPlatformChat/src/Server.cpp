#include "Server.h"

#include <iostream>
#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

namespace Engine
{
	void Server::setDefault() {
		this->m_socketInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	void Server::showMessage(const std::string& t_message) {
		if (this->m_onMessage != nullptr) {
			std::lock_guard<std::mutex> locker(this->m_mu);
			this->m_onMessage(t_message);
		}
	}

	Server::Server() : NetworkUnit() {
		setDefault();
	}

	Server::Server(const int& port) : NetworkUnit(port) {
		setDefault();
	}

	Server::Server(const int& port, const int& connectionSize) : NetworkUnit(port),
	m_connectionSize(connectionSize) {
		setDefault();
	}

	void Server::setOnMessage(const std::function<void(const std::string&)>& function) {
		m_onMessage = function;
	}

	void Server::runServer() {
		this->createServer();
		this->bindServer();
		this->listenServer();
		this->handleRequest();
	}

	void Server::createServer() {
		const int opt = 1;
		this->createSocket();
		if (setsockopt(this->m_socketFd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt))) {
			std::cout << "setsockopt error!" << std::endl;
			std::getchar();
			std::getchar();
			throw std::runtime_error("setsockopt error!");
		}
	}

	void Server::bindServer() {
		if (bind(this->m_socketFd, reinterpret_cast<sockaddr*> (&this->m_socketInfo), this->m_addressLen) == -1) {
			std::cout << "Could not bind socket" << std::endl;
			std::getchar();
			std::getchar();
			throw std::runtime_error("Could not bind socket");
		}
	}

	void Server::listenServer() {
		if (listen(this->m_socketFd, this->m_connectionSize) == -1) {
			std::cout << "Could not open socket for listening" << std::endl;
			std::getchar();
			std::getchar();
			throw std::runtime_error("Could not open socket for listening");
		}
	}

	void Server::handleRequest() {
		showMessage("Server is running...");
		showMessage(std::string(inet_ntoa(this->m_socketInfo.sin_addr)) + ":" + std::to_string(ntohs(this->m_socketInfo.sin_port)));

		while (true) {
			struct sockaddr_in temp_addr;
			socklen_t temp_len = sizeof(temp_addr);
			int temp_socket = accept(this->m_socketFd, reinterpret_cast<sockaddr*> (&temp_addr), &temp_len);

			// check connection limit
			if (this->m_nodeSize + 1 > this->m_connectionSize) {
				sendMessage(temp_socket, "Server is full.");
				#ifdef _WIN32
				closesocket(temp_socket);
				#else
				close(temp_socket);
				#endif

				continue;
			}
			this->m_nodeSize += 1;

			// create client
			NetworkUnit* new_client = new NetworkUnit();
			new_client->setSocketInfo(temp_addr);
			new_client->setSocketFd(temp_socket);
			new_client->setState(STATE::OPEN);

			std::string client_ip(inet_ntoa(temp_addr.sin_addr));
			std::string client_port(std::to_string(ntohs(temp_addr.sin_port)));

			// create thread
			this->v_thread.push_back(std::thread([=] {
				int len;
				char message_buffer[MESSAGE_SIZE];
				while ((len = recv(new_client->getSocketFd(), message_buffer, this->getMessageSize(), 0)) > 0) {
					message_buffer[len] = '\0';
					std::string message(client_ip + ":" + client_port + "> " + message_buffer);
					this->showMessage(message);
					this->sendMessageSpecific(message, new_client->getSocketFd());
					std::memset(message_buffer, '\0', sizeof(message_buffer));
				}
				// connection closed.
				this->v_client.erase(std::remove(this->v_client.begin(), this->v_client.end(), new_client));
				this->showMessage(client_ip + ":" + client_port + " disconnected.");
				}));

			this->showMessage(client_ip + ":" + client_port + " connected.");
			this->v_client.push_back(new_client);
		}
	}

	void Server::sendMessageAll(const std::string& message) {
		for (auto& client : v_client) {
			sendMessage(client->getSocketFd(), message);
		}
	}

	void Server::sendMessageSpecific(const std::string& message, const int& socket) {
		for (auto& client : v_client) {
			if (client->getSocketFd() != socket) {
				sendMessage(client->getSocketFd(), message);
			}
		}
	}

	void Server::closeClients() {
		for (auto& client : v_client) {
			delete client;
		}
	}

	void Server::joinThread() {
		for (auto& thread : v_thread) {
			if (thread.joinable()) {
				thread.join();
			}
		}
	}

	void Server::closeServer() {
		sendMessageAll("Server closed.");
		this->closeClients();
		this->closeSocket();
		this->joinThread();
		this->v_client.clear();
		this->v_thread.clear();
	}

	Server::~Server() {
		this->closeServer();
	}
}
