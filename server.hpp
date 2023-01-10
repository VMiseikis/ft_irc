#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <set>

#define IRC_MESSAGE_LENGHT 512

class Server
{
	private:
		struct sockaddr_in _address;
		int _server;
		int _client;
		int _addrlen;

		struct pollfd _pollfd;
		std::vector<struct pollfd> _pollfds;
		std::set<int> _clients;

		void new_Server();
		
		void store_pollfd(int socket);

	public:
		Server();
		~Server();

		void run_Server();
};

#endif
