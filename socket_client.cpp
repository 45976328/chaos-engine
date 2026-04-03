#include "socket_client.h"
using namespace std;

SocketClient::SocketClient(int port,const string ip){
	clientSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if(clientSocket < 0){
		cerr << "Client: Failed to create socket\n";
		exit(1);
	}

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	if(inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr) <=0){
		cerr << "Client: Invalid IP adress\n";
		close(clientSocket);
		exit(2);
	}
}

SocketClient::~SocketClient(){
	//close(clientSocket);
}

void SocketClient::cnct(){
	if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) <0){
		if (errno == EINPROGRESS) {
            // Connection is in progress — not an error!
            return;
        } else {
            cerr << "Client: Connection failed\n";
            close(clientSocket);
			//exit(3);
		}
	}
}