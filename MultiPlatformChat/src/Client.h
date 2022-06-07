#pragma once

#include "NetworkUnit.h"

#include <iostream>
#include <functional>
#include <thread>
#include <mutex>

namespace Engine
{

	class Client : protected NetworkUnit
	{
	public:
		Client();
		Client(const int& port);
		Client(const std::string& serverName, const int& port);

		void setOnMessage(const std::function<void(const std::string&) >& function);

		void connectServer();
		void closeConnection();

		virtual ~Client();

	protected:
		void setDefault();

		// message event function
		std::function<void(const std::string& message) > m_onMessage = nullptr;
		void showMessage(const std::string& message);

		// server
		std::string m_serverName = "localhost";

		// thread
		std::thread m_thread;
		std::mutex m_mu;

		void _connectServer();
		void receiveMessage();
		void writeMessage();
	};

}