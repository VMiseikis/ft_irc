#include "server.hpp"

//Reikia default porta tureti nusirodziu, del viso pikto
//#define PORT 	4242		// server reachable via this port
#define BACKLOG 0xFFFFFFF 	// the maximum number of pending connections that can be queued up for the socket before connections are refused
#include <arpa/inet.h>		//for inet_addr() 



Server::Server(int port, std::string password) : _port(port), _password(password)
{
	memset(&_address, 0, sizeof(_address));
	memset(&_pollfds, 0, sizeof(_pollfds));
	_conn = -1;
	new_server();
	_cmd = new Commands(this);
}

Server::~Server() {}

std::string Server::get_password() { return _password; }

void Server::new_server()
{
	/*
		socket(domain, service, protocol)
		domain		- AF_INET - IPv4 Internet protocols == TCP/IP
		service 	- SOCK_STREAM - Provides sequenced, reliable, two-way, connection-based byte streams. 
		protocol 	- The protocol specifies a particular protocol to be used with the socket.
					Normally only a single protocol exists to support a particular socket type 
					within a given protocol family, in which case protocol can be specified as 0. 
	*/

	_server = socket(AF_INET, SOCK_STREAM, 0);
	if (_server < 0)
		exit(-1); //TODO error handling
	
	/*
		SOL_SOCKET		- When retrieving a socket option, or setting it, you specify the option name
						as well as the level. When level = SOL_SOCKET, the item will be searched for
						in the socket itself.
		SO_REUSEADDR	- When the listening socket is bound to INADDR_ANY with a specific port 
						then it is not possible to bind to this port for any local address.
						It allows the server to reuse (accept connections) the same ip and port,
						while it's in close-wait or time-wait state.
		enable			- SO_REUSEADDR is a boolean option. It only has two defined values, 0 (off) and 1 (on).
	*/

	const int enable = 1;
	if (setsockopt(_server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		exit(-1); //TODO error handling
	/*
		F_SETFL		- Set the file status flags.
		O_NONBLOCK	- When possible, the file is opened in nonblocking mode. 
		 			Non of I/O operations on the file descriptor which is returned
					will cause the calling process to wait.
		Requirement from sbject
	*/
	if (fcntl(_server, F_SETFL, O_NONBLOCK))
		exit(-1); //TODO error handling

	/*
		define address structure
		INADDR_ANY = (0.0.0.0) means any IPv4 address for binding;
		htons(), htonl 	- convert values between host and network byte order.
						“convert values between host and network byte order”,
						where “Network byte order is big endian, or most significant
						byte first.” 
	*/
	_address.sin_family = AF_INET; 
	// _address.sin_port = htons(PORT);
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);

	
	//Bind socket to the IP address and port
	if (bind(_server, (sockaddr *)&_address, sizeof(_address)) < 0)
		exit(-1); //TODO error handling

	/*
		listen() marks the socket referred to by sockfd as a passive
       	socket, that is, as a socket that will be used to accept incoming
       	connection requests using 
	*/

	if (listen(_server, BACKLOG) < 0)
		exit(-1); //TODO error handling

	store_pollfd(_server);
	
}

void Server::store_pollfd(int socket)
{
	memset(&_pollfd, 0, sizeof(_pollfd));
	_pollfd.fd = socket;
	_pollfd.events = POLLIN;
	_pollfd.revents = 0;
	_pollfds.push_back(_pollfd);
}

void Server::new_connection()
{
	/*
		Accept all incoming connections that are
		queued up on the listening socket before we
		loop back and call poll again. 
	*/

	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);

	while (true)
	{
		memset(&client_address, 0, addrlen);

		_conn = accept(_server, (sockaddr *)&client_address, (socklen_t*)&addrlen);

		if (_conn < 0)
			break;	//TODO error handling
			
		if (fcntl(_conn, F_SETFL, O_NONBLOCK))
			break; 	//TODO error handling
		
		store_pollfd(_conn);
		_connections.insert(std::make_pair(_conn, new Client(_conn, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port))));
		
		std::cout << "Client Connected" << std::endl;		//TODO handle client connect event
	}
}

void Server::get_arguments(std::string line, std::vector<std::string> *args)
{
	(*args).clear();

	for (size_t i = 0; i < line.length(); )
	{
		i = line.find_first_not_of(' ' , i);
		if (line[i] == ':')
		{
			(*args).push_back(line.substr(i, line.size()));
			break;
		}
		(*args).push_back(line.substr(i, line.find(' ', i) - i));
		i += (*args).back().length();
	}

	// for (size_t i = command_name.length(); i < line.length(); )
	// {
	// 	i = line.find_first_not_of(' ' , i);
	// 	if (line[i] == ':')
	// 	{
	// 		i = line.find_first_not_of(' ' , i + 1);
	// 		(*args).push_back(line.substr(i, line.size()));
	// 		break;
	// 	}
	// 	(*args).push_back(line.substr(i, line.find(' ', i) - i));
	// 	i += (*args).back().length();
	// }
	for (std::vector<std::string>::iterator it = (*args).begin(); it != (*args).end(); ++it)
		std::cout << ">>>" <<  *it << "\n";
	std::cout << std::endl;
}

void Server::handle_message(Client *client, std::string message)
{
	std::stringstream ss(message);
	std::string line;
	std::string command_name;
	std::vector<std::string> args;
	
	if (!message.empty())
	{
		while (std::getline(ss, line))
		{
			if (line.back() == '\r')
				line.pop_back();

			for (int i = 0; line[i]; i++)	
				if (!isprint(line[i]))
					break ; 				//TODO handle incorect password format
			
			command_name = line.substr(0, line.find(' '));
			// if (command_name == "CAP")
			// 	continue ;

			get_arguments(line, &args);
			if (!_cmd->execute_command(client, command_name, args)) //jeigu tokios komandos neradome, reiskia, kad tai tik paprasta zinute, todel turim jabroadcastinti i kanala ar kazkas panasaus
				break ; // broadcast message or handle different way, idk
			// _cmd->execute_command(this, client, command_name, args);
			
			if (!client->get_nick_name().empty() && !client->get_user_name().empty() && client->get_status() == HANDSHAKE)
			{
				std::string welcome_message = ":MultiplayerNotepad 001 " + client->get_nick_name() + " :Welcome to MultiplayerNodepad " + client->get_nick_name() + "\r\n";
				send(client->get_fd(), welcome_message.c_str(), welcome_message.length(), 0);
				client->set_status(REGISTERED);	
			}

			// if (client->get_status() == NEW && command_name == "PASS")
			// {
			// 	_cmd->execute_command(command_name, args);
			// 	client->set_status(HANDSHAKE);
			// }
			// else if (client->get_status() == HANDSHAKE 
			// 	&& (command_name == "NICK" || command_name == "USER"))
			// {
			// 	_cmd->execute_command(command_name, args);
			// 	if (!client->get_nick_name().empty() && !client->get_user_name().empty())
			// 	//  && !client->get_password().empty()) //kol kas nereikalingas nes tikrinam tik serverio passworda
			// 	{
			// 		client->set_status(REGISTERED);
			// 		std::string welcome_message = ":MultiplayerNotepad 001 " + client->get_nick_name() + " :Welcome to MultiplayerNodepad " + client->get_nick_name() + "\r\n";
			// 		send(client->get_fd(), welcome_message.c_str(), welcome_message.length(), 0);
			// 	}
			// }
			// else if (!_cmd->execute_command(command_name, args)) //jeigu tokios komandos neradome, reiskia, kad tai tik paprasta zinute, todel turim jabroadcastinti i kanala ar kazkas panasaus
			// {
			// 	// broadcast message or handle different way, idk
			// }			
		}
	}
}

void Server::message_recieved(int fd)
{
	/*
		RFC 2812
		IRC messages are always lines of characters terminated 
		with a CR-LF (Carriage Return - Line Feed) pair, and 
		these messages SHALL NOT exceed 512 characters in length,
		counting all characters including the trailing CR-LF. 
		Thus, there are 510 characters maximum allowed for the 
		command and its parameters.
	*/

	char buffer[IRC_MESSAGE_LENGHT];
	memset(&buffer, 0, IRC_MESSAGE_LENGHT);

	if (recv(fd, buffer, IRC_MESSAGE_LENGHT, 0) > 0 && strstr(buffer, "\r\n"))
	{
		try {
			std::cout << buffer << std::endl;

			handle_message(_connections.at(fd), buffer);

		} catch (const std::out_of_range &err) {}
	}
} 

void Server::run_server()
{
	std::vector<struct pollfd>::iterator it;
	
	while (true)
	{
		if (poll(_pollfds.begin().base(), _pollfds.size(), -1) < 0)
			std::cout << "ERR POLL" << std::endl;					//TODO handle poll error

		for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
		{
			
			if (it->revents == 0)
				continue;

			if (it->revents != POLLIN)
			{
				if (it->revents == POLLNVAL)
					std::cout << "ERR" << std::endl; 					//TODO error handling (Invalid request: fd not open)
				else if (it->revents == POLLHUP)
					std::cout << "Disconected" << std::endl; 			//TODO handle client disconect event
				break ;
			}

			if (it->fd == _server)
				new_connection();
			else
				message_recieved(it->fd);
		}
	}
}

std::vector<Channel *> & Server::getChannels(void)	{
	return (_channels);
}

Channel	*Server::getChannel(std::string &name)	{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it < _channels.end(); it++)	{
		if ((*it)->getName() == name)
			return (*it);
	}
	return (NULL);
}
