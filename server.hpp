#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <cctype>

#include <poll.h>
#include <vector>
#include <map>

#include "client.hpp"




#define IRC_MESSAGE_LENGHT 512
// #define HOST_NAME_MAX 63

class Server
{
	private:
		struct sockaddr_in _address;
		int _server;
		int _port;
		std::string _password;
		int _client;		//connecting client socket

		struct pollfd _pollfd;
		std::vector<struct pollfd> _pollfds;	
		std::map<int, Client *> _clients;		//list of all conected clients

		void new_server();
		void store_pollfd(int socket);

	public:
		Server(int port, std::string password);
		~Server();

		void run_server();
};

#endif
