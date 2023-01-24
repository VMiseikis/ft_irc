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
#include "channel.hpp"




#define IRC_MESSAGE_LENGHT 64
// #define HOST_NAME_MAX 64

class Commands;

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
		

		std::map<int, Client *> _clients;		//list of all connections/clients


		Commands *_cmd;							//list of commands

		std::vector<Channel *>	_channels;

		void new_server();
		void store_pollfd(int socket);

	public:
		Server(int port, std::string password);
		~Server();

		void get_arguments(std::string line, std::vector<std::string> *args);
		// void get_arguments(std::string line, std::string command_name, std::vector<std::string> *args);
		std::string get_password();


		void run_server();
		void new_connection();
		void message_recieved(int fd);
		void handle_message(Client *client, std::string message);

		void client_disconnect(std::vector<struct pollfd>::iterator poll_it);

		Client *get_client(std::string name);
		std::vector<Channel *> & getChannels(void);
		Channel	*getChannel(std::string	&name);
};

#endif
