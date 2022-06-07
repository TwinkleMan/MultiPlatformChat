#pragma once

#include "NetworkUnit.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <thread>
#include <mutex>

namespace Engine
{

	class Server : protected NetworkUnit
	{
	public:
		Server();
		Server(const int& port);
		Server(const int& port, const int& connectionSize);

		void setOnMessage(const std::function<void(const std::string&) >& function);

		void runServer();
		void closeServer();

		virtual ~Server();

	protected:
		void setDefault();

		// message event function
		std::function<void(const std::string& message) > m_onMessage = nullptr;
		void showMessage(const std::string& message);

		// thread
		std::vector<std::thread> v_thread;
		void joinThread();
		std::mutex m_mu;

		// client
		int m_nodeSize = 0;
		std::vector<NetworkUnit*> v_client;
		void closeClients();

		// server
		int m_connectionSize = 128;
		void createServer();
		void bindServer();
		void listenServer();
		void handleRequest();

		// Message
		void sendMessageAll(const std::string& message);
		void sendMessageSpecific(const std::string& message, const int& socket);
	};

}
