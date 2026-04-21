#include "proxy_server.h"

Connection::Connection(int frontfd): back(5051,"127.0.0.1"){
	this->frontfd = frontfd;
	back.cnct();
	backfd = back.getConnectSocket();

}

Connection::Connection(int frontfd, int port, char* backip): back(port, backip){
	this->frontfd = frontfd;
	back.cnct();
	backfd = back.getConnectSocket();

}

Connection::~Connection(){}

int Connection::readfront(){
    if (ftbbytes != 0) return 0; // buffer still has unsent data
    ftb_offset = 0;
	ftbbytes = recv(frontfd, front_to_back_buff, BUFFER_SIZE, 0);
	request = true;
	ce.dispatch(); // chaos engine dispatch
	request = false;
    return ftbbytes;
}

int Connection::readback(){
    if (btfbytes != 0) return 0; // buffer still has unsent data
    btf_offset = 0;
	request = true;
	ce.dispatch(); // chaos engine dispatch
	request = false;
    return btfbytes = recv(backfd, back_to_front_buff, BUFFER_SIZE, 0);
}

int Connection::writefront(){
	response = true;
	ce.dispatch();// chaos engine dispatch
	response = false;
    int sent = send(frontfd, back_to_front_buff + btf_offset, btfbytes, 0);
    if (sent > 0){
        btf_offset += sent;
        btfbytes -= sent;
        if (btfbytes == 0) btf_offset = 0;
    }
    return sent;
}

int Connection::writeback(){
	response = true;
	ce.dispatch();// chaos engine dispatch
	response = false;
    int sent = send(backfd, front_to_back_buff + ftb_offset, ftbbytes, 0); // TODO https://man7.org/linux/man-pages/man2/send.2.html FIND OUT IF ftbbytes affects send
    if (sent > 0){
        ftb_offset += sent;
        ftbbytes -= sent;
        if (ftbbytes == 0) ftb_offset = 0;
    }
    return sent;
}


Epoll::Epoll(){
	epollfd = epoll_create1(0);
	if (epollfd == -1){
		cout << "ERROR: epoll_create1()\n";
		exit(EXIT_FAILURE);
	}
}

Epoll::~Epoll(){}

int Epoll::wait(){
	int eventsready = epoll_wait(epollfd, events, MAX_EVENTS, -1);

	if(eventsready == -1){
		cout << "ERROR: epoll_wait()\n";
	}
	return eventsready;
}

void Epoll::addfd(int fd, int flag){ // fd, EPOLLIN EPOLLOUT // TODO add returns -1, 1
	struct epoll_event ev;
	ev.events = flag;
	ev.data.fd = fd;

	if ( epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1 ){
		cout << "ERROR: Could not add entry to the interest list of epfd\n";
	}
}

void Epoll::modfd(int fd, int flag){
	struct epoll_event ev;
	ev.events = flag;
	ev.data.fd = fd;

	if ( epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev) == -1 ){
		cout << "ERROR: Count not change settings associated with fd in the interest list\n";
	}
}

void Epoll::removefd(int fd){
	if ( epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1 ){
		cout << "ERROR: Could not deregister target file descriptor from the interest list";
         }
}

ProxyServer::ProxyServer(int port, char* backip){
	this->port = port;
	this->backip = backip;

	front.startListening();
	
	epoll.addfd(front.getListeningSocket(), EPOLLIN);

}

ProxyServer::~ProxyServer(){}

void ProxyServer::clean(struct epoll_event event){
	// Check if already cleaned
    if (fdmap.find(event.data.fd) == fdmap.end()){
        return;
	}
	Connection* conn = fdmap[event.data.fd];

    int frontfd = conn->getfrontfd();
    int backfd  = conn->getbackfd();

    // Remove from epoll
    epoll.removefd(frontfd);
    epoll.removefd(backfd);

    // Close sockets
    close(frontfd);
    close(backfd);

    // Remove both entries from map
    fdmap.erase(frontfd);
    fdmap.erase(backfd);

    // Free memory
    delete conn;
}

void ProxyServer::addconnections(){
	while(true){
		int clientfd = front.acceptClient();

		if (clientfd == -1){
			if (errno == EAGAIN || errno == EWOULDBLOCK){
				break; // no more pending clients to accept
			}else if (errno == EINTR){
				continue; //Interrupted by signal, retry accept
			}else{ // Actual error
				cout << "ERROR: accept\n";
				break; // return;
			}
		}

		Connection* conn = new Connection(clientfd); //also creates a connection to backend

		fdmap[clientfd] = conn;
		
		fdmap[conn->getbackfd()] = conn; 

		// TODO check for errors on add, add close(fd) continue; if -1
		epoll.addfd(clientfd, EPOLLIN); //Register newly accepted client with epoll
		epoll.addfd(conn->getbackfd(), EPOLLIN);

		// One proxied session has 2 sockets: frontfd (client side) and backfd (backend side).
		// We store BOTH fds in fdmap pointing to the same Connections object,
		// so when epoll returns events[i].data.fd we can instantly find the owning
		// connection and then check whether that fd is the front or back side.
		// 2 entries with the same object but the key (int) is frontfd on one and backfd on the other 
	}
}

void ProxyServer::mainloop(){
	struct epoll_event* events = epoll.getevents();
	while(true){
		
		eventsready = epoll.wait();

		for (int i = 0; i<eventsready; i++){
			//TODO likely fix on segmentation fault bug
			// auto it = fdmap.find(events[i].data.fd);
			// if (it == fdmap.end())
			// 	continue;
			// Connection* conn = it->second;

			if (events[i].data.fd == front.getListeningSocket()){ // check if event triggered is Listening Socket fd
				this->addconnections();
			}else{
				if (events[i].events & EPOLLIN){ //if you get EPOLLIN, find if its front or back then read
					if (fdmap[events[i].data.fd]->getbackfd() == events[i].data.fd){ //Backfd readable

						int bytes = fdmap[events[i].data.fd]->readback();
						if (bytes > 0){ //enable EPOLLOUT on opposite socket
							epoll.modfd(fdmap[events[i].data.fd]->getfrontfd(), EPOLLIN | EPOLLOUT);							
						}else if (bytes == 0){// peer closed cleanup connection
							//remove front and back fd from interest list close fds remove from fdmap
							clean(events[i]);
    						continue;
						}else{
							if (errno == EAGAIN){ //no more bytes available right now, dont disable EPOLLOUT
								continue;
							}else{
								//cleanup connection
								//remove front and back fd from interest list close fds remove from fdmap	
								clean(events[i]);
    							continue;						
							}
						}
						
					}else{//frontfd readable
						int bytes = fdmap[events[i].data.fd]->readfront();

						if (bytes > 0){ //enable EPOLLOUT on opposite socket
							epoll.modfd(fdmap[events[i].data.fd]->getbackfd(), EPOLLIN | EPOLLOUT);
						}else if (bytes == 0){// peer closed cleanup connection
							clean(events[i]);
    						continue;
						}else{
							if (errno == EAGAIN){ //no more bytes available right now, dont disable EPOLLOUT
								continue;
							}else{
								//cleanup connection
								clean(events[i]);
    							continue;
							}
						}
					}
				
				}else if (events[i].events & EPOLLOUT){
					if (fdmap[events[i].data.fd]->getbackfd() == events[i].data.fd){//Backfd writable
						int bytes = fdmap[events[i].data.fd]->writeback();
						if (bytes == 0){ // TODO disable EPOLLOUT only when buff is empty // and ftbbytes == 0 ?
							epoll.modfd( fdmap[events[i].data.fd]->getbackfd(), EPOLLIN );
						}else{
							if (errno == EAGAIN){ //no more bytes available right now
								continue;
							}else{
								//cleanup connection
								clean(events[i]);
    							continue;
							}
						}

					}else{//frontfd writable
						int bytes = fdmap[events[i].data.fd]->writefront();
						if (bytes == 0){ 
								epoll.modfd( fdmap[events[i].data.fd]->getfrontfd(), EPOLLIN );
						}else{
							if (errno == EAGAIN){ //no more bytes available right now
								continue;
							}else{
								//cleanup connection
								clean(events[i]);
    							continue;
							}
						}
					}

				}else{ // events[i].events & (EPOLLERR | EPOLLHUP)
					//Socket broken ... close it
					clean(events[i]);
    				continue;
				}
			}
			//cout << "FD: " << events[i].data.fd << " EVENT: " << events[i].events << endl;
		}
	}	
}