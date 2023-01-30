#include "server.hpp"

//Reikia default porta tureti nusirodziu, del viso pikto
//#define PORT 	4242		// server reachable via this port
#define BACKLOG 0xFFFFFFF 	// the maximum number of pending connections that can be queued up for the socket before connections are refused
#include <arpa/inet.h>		//for inet_addr() 



Server::Server(int port, std::string password) : _port(port), _password(password)
{
	_oper_name = "admin";
	_oper_pass = "admin";
	memset(&_address, 0, sizeof(_address));
	memset(&_pollfds, 0, sizeof(_pollfds));
	_conn = -1;
	new_server();
	_cmd = new Commands(this);
	_name = "MultiplayerNotepad";
}

Server::~Server() {}

std::string Server::get_password() { return _password; }
std::string Server::get_oper_name() { return _oper_name; }
std::string Server::get_oper_pass() { return _oper_pass; }

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
	if (fcntl(_server, F_SETFL, O_NONBLOCK) < 0)
		exit(-1); //TODO error handling

	/*
		define address structure
		INADDR_ANY = (0.0.0.0) means any IPv4 address for binding;
		htons(), htonl 	- convert values between host and network byte order.
						“convert values between host and network byte order”,
						where “Network byte order is big endian, or most significant
						byte first.” 
	*/
	memset(&_address, 0, sizeof(_address));
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
	// struct pollfd _pollfd = {_server, POLLIN, 0};
	// _pollfds.push_back(_pollfd);
	
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
		char hostname[NI_MAXHOST];
		getnameinfo((struct sockaddr *) &client_address, sizeof(client_address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV); //TODO != 0 error handling

		std::cout << "FD:" << _conn <<std::endl;
		_clients.insert(std::make_pair(_conn, new Client(_conn, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), hostname)));
		
		std::cout << "Client Connected" << std::endl;		//TODO handle client connect event
	}
}

void Server::get_arguments(std::string line, std::vector<std::string> *args)
{
	(*args).clear();

	for (size_t i = 0; i < line.length(); )
	{
		i = line.find_first_not_of(' ' , i);
		if (i == std::string::npos)
			break ;
		if (line[i] == ':')
		{
			i = line.find_first_not_of(' ' , i + 1);
			(*args).push_back(line.substr(i, line.size()));
			break ;
		}
		(*args).push_back(line.substr(i, line.find(' ', i) - i));
		i += (*args).back().length();
	}
}

void Server::handle_message(Client *client, std::string message)
{
	size_t begin, end;
	std::stringstream ss(message);
	std::string line = "";
	std::vector<std::string> args;

	if (!message.empty())
	{
		while (std::getline(ss, line))
		{
			begin = line.find_first_not_of(WHITESPACES);
			end = line.find_last_not_of(WHITESPACES) + 1;
			if(begin != std::string::npos && end != std::string::npos)
				line = line.substr(begin, end);
			// else
			// 	line = "";
					
			if (!line.empty())
			{
				for (int i = 0; line[i]; i++)	
					if (!isprint(line[i]))
						break ; 				//TODO handle incorect password format
				_cmd->execute_command(client, line);	
			}
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

	std::string msg;

	char buffer[IRC_MESSAGE_LENGHT];
	memset(&buffer, 0, IRC_MESSAGE_LENGHT);
	while(!strstr(buffer, "\r\n"))
	{
		memset(&buffer, 0, IRC_MESSAGE_LENGHT);
		if(recv(fd, &buffer, IRC_MESSAGE_LENGHT - 1, 0) < 0)
			break ;
		msg.append(buffer);
	}

	try {
		std::cout << "BUFF>>" << msg << std::endl;
		handle_message(_clients.at(fd), msg);
	} catch (const std::out_of_range &err) {}

	// if (recv(fd, buffer, IRC_MESSAGE_LENGHT, 0) > 0 && strstr(buffer, "\r\n"))
	// {
	// 	try {
	// 		std::cout << "BUFF>>" << buffer << std::endl;
	// 		handle_message(_clients.at(fd), buffer);
	// 	} catch (const std::out_of_range &err) {}
	// }
} 

Client *Server::get_client(std::string name)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->get_nick_name() == name)
			return it->second;
	return NULL;
}

void Server::client_disconnect(std::vector<struct pollfd>::iterator it)
{
	try {
		_clients.at(it->fd)->dc();
		delete _clients.at(it->fd);
		_clients.erase(it->fd);
		close(it->fd);
		_pollfds.erase(it);
	}
	catch (const std::out_of_range &err) {}
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

			if ((it->revents & POLLHUP) == POLLHUP)
			{
				std::cout << "Disconected" << std::endl; 			//TODO handle client disconect event
				client_disconnect(it);
				break;
			} else if ((it->revents & POLLIN) == POLLIN)
			{
				if (it->fd == _server)
				{
					new_connection();
					break;
				}
				message_recieved(it->fd);
			} 
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
void	Server::deleteChannel(Channel *channel)	{
		std::vector<Channel *>::iterator it;
		for (it = _channels.begin(); it != _channels.end(); it++)	{
			if (*it == channel)
				delete *it;
				_channels.erase(it);
			break ;
		}
		//tetst
		if (_channels.empty())
			std::cout << "no channels exist\n";
		else	{
		for (it = _channels.begin(); it != _channels.end(); it++)	{
			std::cout << (*it)->getName() << " existing servers\n";
		}
		}
	}
std::string	Server::getName(void)	{
	return (_name);
}
