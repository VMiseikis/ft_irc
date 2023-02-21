/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmiseiki <vmiseiki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 16:51:12 by vmiseiki          #+#    #+#             */
/*   Updated: 2023/02/09 16:51:13 by vmiseiki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

bool Server::_on = true; 

Server::Server(int port, std::string password) : _port(port), _password(password) 
{
	_conn = -1;
	_name = "MultiplayerNotepad";
	_admin_name = "admin";
	_admin_pass = "pass";
	_cmd = new Commands(this);
	memset(&_address, 0, sizeof(_address));
	new_server();
}

Server::~Server()
{
	close(_server);
	delete _cmd;

	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;

	for (std::vector<Channel *>	::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete *it;
}

std::string	Server::get_name()			{ return _name; 	  }
std::string Server::get_password() 	 	{ return _password;   }
std::string Server::get_admin_name() 	{ return _admin_name; }
std::string Server::get_admin_pass() 	{ return _admin_pass; }

std::vector<Channel *> &Server::get_channels() { return _channels; }

Channel	*Server::get_channel(std::string &name)
{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it < _channels.end(); it++) {
		std::string	Name = (*it)->getName();
		if (!strncasecmp(Name.c_str(), name.c_str(), Name.length() + 1))
			return (*it);
	}
	return (NULL);
}

bool Server::is_on(void) { return Server::_on; }

void Server::turn_off(int sig)
{
	(void)sig;
	Server::_on = false;
}

void Server::new_server()
{
	_server = socket(AF_INET, SOCK_STREAM, 0);
	if (_server < 0)
		throw (std::runtime_error("Error: Server failed to get a socket\n"));

	const int enable = 1;
	if (setsockopt(_server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw (std::runtime_error("Error: Failed to set socket options\n"));

	if (fcntl(_server, F_SETFL, O_NONBLOCK) < 0)
		throw (std::runtime_error("Error: Failed to set socket to NON-BLOCKING\n"));

	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET; 
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(_server, (sockaddr *)&_address, sizeof(_address)) < 0)
		throw (std::runtime_error("Error: Failed to bind address to socket\n"));

	if (listen(_server, BACKLOG) < 0)
		throw (std::runtime_error("Error: Failed to start listening\n"));

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
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);

	memset(&client_address, 0, addrlen);
	_conn = accept(_server, (sockaddr *)&client_address, (socklen_t*)&addrlen);
	if (_conn < 0){
		std::cerr << "Error: Error ocured while accepting connection\n";
		return ;
	}
	
	if (fcntl(_conn, F_SETFL, O_NONBLOCK)){
		std::cerr << "Error: Failed to set socket to NON-BLOCKING\n";
		return ;
	}

	char hostname[NI_MAXHOST];
	if (getnameinfo((struct sockaddr *) &client_address, sizeof(client_address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0) {
		std::cerr << "Error: Failed to get hostname of the client. IP address will be used instead\n";
		hostname[0] = '\0';
	}

	store_pollfd(_conn);
	_clients.insert(std::make_pair(_conn, new Client(_conn, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), hostname)));
	if (_clients.at(_conn)->get_hostname().empty() || _clients.at(_conn)->get_hostname().size() > 63)
		_clients.at(_conn)->set_hostname(_clients.at(_conn)->get_ip());

	std::cout << BLUE << "New client: " << RESET;
	std::cout << _clients.at(_conn)->get_hostname();
	std::cout << "(" << inet_ntoa(client_address.sin_addr) << ":";
	std::cout << ntohs(client_address.sin_port) <<  ") connected" << std::endl;
}

void Server::handle_message(Client *client, std::string message)
{
	size_t begin, end;
	std::string line = "";
	std::stringstream ss(message);
	std::vector<std::string> args;

	if (!message.empty()) {
		while (std::getline(ss, line)) {
			begin = line.find_first_not_of(WHITESPACES);
			end = line.find_last_not_of(WHITESPACES) + 1;

			if(begin != std::string::npos && end != std::string::npos)
				line = line.substr(begin, end);
					
			if (!line.empty())
				_cmd->execute_command(client, line);	
		}
	}
}

void Server::message_recieved(int fd)
{
	int bytes;
	std::string msg;

	char buffer[BUFFER_LENGHT];
	memset(buffer, 0, BUFFER_LENGHT);

	while(!strstr(buffer, "\r\n")) {
		memset(buffer, 0, BUFFER_LENGHT);
		bytes = recv(fd, buffer, BUFFER_LENGHT - 1, 0);
		if (bytes < 0)
			break ;
		if (bytes == 0)
		{
			client_quit(fd);
			return;
		}
		msg.append(buffer);
	}

	std::cout << msg << std::endl;
	try {
		handle_message(_clients.at(fd), msg);
	}
	catch (const std::out_of_range &err) {
		std::cerr << "Error: Error occured while handling " + _clients.at(fd)->get_hostname() + " message\n";
	}
} 

Client *Server::get_client(std::string name)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->get_nick_name() == name)
			return it->second;
	return NULL;
}

void Server::client_quit(int fd)
{
	for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
		if ((*it).fd == fd)
			return client_disconnect(it);
}

void Server::client_disconnect(std::vector<struct pollfd>::iterator it)
{
	try {
		std::cout << _clients.at(it->fd)->get_id();
		std::cout << RED_C << " disconnected" << RESET << std::endl;
		_clients.at(it->fd)->dc();
		delete _clients.at(it->fd);
		_clients.erase(it->fd);
		_pollfds.erase(it);
	} 
	catch (const std::out_of_range &err) {
		std::cerr << "Error: Error occured during " + _clients.at(it->fd)->get_hostname() + " disconnect\n";
	}
}

void Server::run_server()
{
	std::vector<struct pollfd>::iterator it;
	while (is_on()) {
		if (poll(_pollfds.begin().base(), _pollfds.size(), -1) < 0)	{
			if (_on)
				throw (std::runtime_error("Error: Poll failure"));
			else
				throw (std::runtime_error("\b\bServer turned OFF"));
		}
		for (it = _pollfds.begin(); it != _pollfds.end(); ++it) {
			if (it->revents == 0)
				continue ;

			if ((it->revents & POLLHUP) == POLLHUP) {
				client_disconnect(it);
				break ;
			} 
			else if ((it->revents & POLLIN) == POLLIN) {
				if (it->fd == _server) {
					new_connection();
					break ;
				}
				message_recieved(it->fd);
				break ;
			}
		}
	}
}

void Server::broadcast_to_all_clients(std::string msg)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		std::string message = ":" + _name + " PRIVMSG " + it->second->get_nick_name() + " :" + msg + "\r\n";
		send(it->first, message.c_str(), message.length(), 0);
	}
}

void Server::wall(const std::string &msg)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		send(it->first, msg.c_str(), msg.length(), 0);
}

void Server::delete_channel(Channel *channel)
{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (*it == channel) {
			delete *it;
			_channels.erase(it);
			break ;
		}
	}
}
