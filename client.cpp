#include "client.hpp"

Client::Client(int fd, std::string ip, int port) : _fd(fd), _ip(ip), _port(port) {}

Client::~Client() {}

int Client::get_fd()	{ return _fd; }
int Client::get_port()	{ return _port; }

std::string Client::get_ip() 		{ return _ip; }
std::string Client::get_user_name() { return _name; }

void Client::set_user_name(std::string name) { _name = name; }