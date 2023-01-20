#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <cctype>
#include <cstring>
#include <sstream>
#include <poll.h>
#include <vector>
#include <map>

#include "client.hpp"
#include "commands.hpp"




#define IRC_MESSAGE_LENGHT 512
// #define HOST_NAME_MAX 63

class Server
{
	private:
		struct sockaddr_in _address;
		int _server;
		int _port;
		std::string _password;
		int _conn;		//connecting client socket


		struct pollfd _pollfd;
		std::vector<struct pollfd> _pollfds;
		
		std::vector	<Client *> _clients;			//list of all registered, active clients
		std::map<int, Client *> _connections;		//list of all active connections


		Commands *_cmd;							//list of commands


		void new_server();
		void store_pollfd(int socket);

	public:
		Server(int port, std::string password);
		~Server();

		void get_arguments(std::string line, std::string command_name, std::vector<std::string> args);



		void run_server();
		void new_connection();
		void message_recieved(int fd);
		void handle_message(Client *client, std::string message);
};

#endif
