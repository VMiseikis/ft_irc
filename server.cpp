#include "server.hpp"

//#define PORT 	4242		// server reachable via this port
#define BACKLOG 0xFFFFFFF 	// the maximum number of pending connections that can be queued up for the socket before connections are refused

Server::Server(int port, std::string password) : _port(port), _password(password)
{
	memset(&_address, 0, sizeof(_address));
	memset(&_pollfds, 0, sizeof(_pollfds));
	_client = -1;
	new_Server();
}

Server::~Server() {}

void Server::new_Server()
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

void Server::run_Server()
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
			{
				std::cout << "Client Connected" << std::endl;		//TODO handle client connect event
				/*
					Accept all incoming connections that are
        			queued up on the listening socket before we
        			loop back and call poll again. 
				*/
				char ip[NI_MAXHOST];	//client ip
				//char sv[NI_MAXSERV];	
				struct sockaddr_in client_address;
				int addrlen = sizeof(client_address);

				while (true)
				{
					memset(&client_address, 0, addrlen);
					_client = accept(_server, (sockaddr *)&client_address, (socklen_t*)&addrlen);

					if (_client < 0)
						break;	//TODO error handling
						
					if (fcntl(_client, F_SETFL, O_NONBLOCK))
						break; 	//TODO error handling

					store_pollfd(_client);
					_clients.insert(_client);

					memset(&ip, 0, NI_MAXHOST);
					getnameinfo((sockaddr *)&client_address, addrlen, ip, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				
					// memset(&sv, 0, NI_MAXSERV);
					// getnameinfo((sockaddr *)&client_address, addrlen, ip, NI_MAXHOST, sv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
				}
			}
			else
			{
				std::cout << "Message from client" << std::endl;	//TODO handle messages from client
				/*
					RFC 2812
					IRC messages are always lines of characters terminated 
					with a CR-LF (Carriage Return - Line Feed) pair, and 
					these messages SHALL NOT exceed 512 characters in length,
					counting all characters including the trailing CR-LF. 
					Thus, there are 510 characters maximum allowed for the 
					command and its parameters.
				*/

				// Reikia pagalvt kaip tikrint jeigu client message yra ilgesne, nei bufferis. 
				// Kaip informuot apie tai klienta? Ar geriau loopint kol gaunama visa zinute?

				char buffer[IRC_MESSAGE_LENGHT];
				bzero(buffer, IRC_MESSAGE_LENGHT);
				if (recv(it->fd, buffer, IRC_MESSAGE_LENGHT, 0) > -1)
					std::cout << buffer << std::endl; 
				else
					break ;	//TODO error handling
			}
		}
	}
}
