#include "server.hpp"

#define PORT 	4242		// server reachable via this port
#define BACKLOG 0x7FFFFFF 	// the maximum number of pending connections that can be queued up for the socket before connections are refused

Server::Server()
{
	newServer();
}

Server::~Server() {}

void Server::newServer()
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
	if (setsockopt(_server, SOL_SOCKET,  SO_REUSEADDR, &enable, sizeof(int) < 0))
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
	_address.sin_port = htons(PORT);
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
}

Server::runServer()
{
	while (true)
	{
		//Client Connect

		//Client Disconnect

		//Client Message
	}
}
