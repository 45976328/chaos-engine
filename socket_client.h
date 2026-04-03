#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;
class SocketClient{
	private:
		int clientSocket;
		sockaddr_in serverAddress;
	public:
		SocketClient(int,const string);
		~SocketClient();

		int getConnectSocket(){return clientSocket;}
		void cnct();
};


#endif
