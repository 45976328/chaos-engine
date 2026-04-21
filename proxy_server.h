#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#include <sys/epoll.h>
#include <errno.h>
#include <chrono>
#include <unordered_map> // https://www.geeksforgeeks.org/cpp/map-vs-unordered_map-c/

#include "socket_server.h"
#include "socket_client.h"
#include "chaos_engine.h"

#define MAX_EVENTS 64
#define BUFFER_SIZE 4096

class Connection{
	private:
		SocketClient back;

		int frontfd;
		int backfd;

		char front_to_back_buff[BUFFER_SIZE];
		int ftbbytes = 0;
		int ftb_offset = 0;
		char back_to_front_buff[BUFFER_SIZE];
		int btfbytes = 0;
		int btf_offset = 0;

		bool request = false; //TODO PASS DIRECTION TO CHAOS ENGINE
		bool response = false;

		//CHRONO now() + release times + flags? (ce points to these variables since we have void on all functions) 
		// On read, I ask ChaosEngine if delay applies. (dispatcher)
		// If yes, I store a release timestamp in the Connection.
		// On write, I check the timestamp before sending.”

		ChaosEngine ce{front_to_back_buff, back_to_front_buff, request, response};

	public:
		Connection(int);
		Connection(int, int, char*);
		~Connection();

		int getbackfd() const {return backfd;}
		int getfrontfd() const {return frontfd;}

		int getftbbytes() const { return ftbbytes; }
		int getbtfbytes() const { return btfbytes; }

		int readfront();
		int writefront();

		int readback();
		int writeback();
		
		void clean(){close(frontfd); close(backfd);}
};

class Epoll{
	private:
		struct epoll_event events[MAX_EVENTS];
		int epollfd;

	public:
		Epoll();
		~Epoll();

		int getEpollfd() const {return epollfd;}
		int wait();
		void addfd(int, int);
		void modfd(int, int);
		void removefd(int);
		struct epoll_event* getevents(){ return events;}
};


class ProxyServer{
	private:
		int port;
		char* backip;

		int eventsready;

		Epoll epoll;

		SocketServer front{5050};
				
		unordered_map<int,Connection*> fdmap;
	public:
		// ProxyServer();
		ProxyServer(int, char*);
		~ProxyServer();

		void addconnections();

		void clean(struct epoll_event);

		void mainloop();
};



#endif
