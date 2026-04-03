#include "socket_server.h"
using namespace std;

SocketServer::SocketServer(int port){
	serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (serverSocket < 0){
		cerr << "Failed to create socket\n";
		exit(1);
	}

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET; //IPv4 protocol
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if( bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress) ) < 0 ){
		cerr << "Bind failed\n";
		exit(2);
	}
}

SocketServer::~SocketServer(){
	close(serverSocket);
}

void SocketServer::startListening(int backlog){
	if (listen(serverSocket, backlog) < 0){
		close(serverSocket);
		exit(3);
	}
}

int SocketServer::acceptClient(){
	int clientSocket = accept4(serverSocket, nullptr, nullptr, SOCK_NONBLOCK);
	if(clientSocket < 0){
		//cerr << "Accept failed\n";
	}
	return clientSocket;
}
