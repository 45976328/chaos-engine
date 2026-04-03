#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


class SocketServer{
	private:
		int serverSocket;
		sockaddr_in serverAddress;
	public:
		SocketServer(int);
		~SocketServer();

		int getListeningSocket(){return serverSocket;}
		void startListening(int backlog = 10);
		int acceptClient(); //accept and return socket fd


};
#endif
