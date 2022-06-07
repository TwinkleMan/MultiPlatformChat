#include <iostream>

#include <limits>
#include <csignal>
#include <string>

#include "Client.h"
#include "Server.h"

Engine::Client* chatClient = nullptr;
Engine::Server* chatServer = nullptr;

void signalHandler(int);
void printMessage(const std::string&);

void client()
{
	signal(SIGINT, signalHandler);

	std::string server = "localhost";
	std::cout << "Server address (default - localhost):\n";
	std::getchar();
	std::getline(std::cin, server);
	std::cout << "Port (default - 5000): ";
	int port = 5000;
	std::cin >> port;

	try {

		chatClient = new Engine::Client(server, port);
		chatClient->setOnMessage(printMessage);
		chatClient->connectServer();
		delete chatClient;

	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

void server()
{
	signal(SIGINT, signalHandler);

	int port = 5000;
	std::cout << "Port (default - 5000): ";
	std::cin >> port;
	std::cout << "Number of connections (default - 16 devices): ";
	int connection_size = 16;
	std::cin >> connection_size;

	try {

		chatServer = new Engine::Server(port, connection_size);
		chatServer->setOnMessage(printMessage);
		chatServer->runServer();
		delete chatServer;

	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

int main(int argc, char** argv) {

	int mode;

	std::cout << "Select work mode\n1)Client\n2)Server" << std::endl;
	std::cin >> mode;

	switch (mode)
	{
	case 1:
		client();
		break;
	case 2:
		server();
		break;
	}

	return 0;
}

void signalHandler(int code) {
	char ch;
	std::cout << "Are you sure you want to close socket?(Y/N)";
	std::cin >> ch;
	if (toupper(ch) == 'Y' && chatClient != nullptr) {
		delete chatClient;
		exit(0);
	}
	std::cin.clear();
	std::cin.ignore(2048, '\n');
}

void printMessage(const std::string& message) {
	std::cout << message << std::endl;
}
