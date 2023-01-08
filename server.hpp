#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

class Server
{
	private:
		struct sockaddr_in _address;
		int _server;

	public:
		
		Server();
		~Server();
		void newServer();
		void runServer();
};

#endif
