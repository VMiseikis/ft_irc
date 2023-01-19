#include "client.hpp"


Client::Client(int fd, std::string ip, int port) : _fd(fd), _port(port), _status(HANDSHAKE), _ip(ip), _name(), _nick(), _pass() {}

Client::~Client() {}

int Client::get_fd()	{ return _fd; }
int Client::get_port()	{ return _port; }
int Client::get_status() { return _status; }
std::string Client::get_ip() 		{ return _ip; }
std::string Client::get_user_name() { return _name; }
std::string Client::get_nick_name() { return _nick; }
std::string Client::get_password()	{ return _pass; }

void Client::set_status(int status) { _status = status; }
void Client::set_user_name(std::string name) { _name = name; }
void Client::set_nick_name(std::string nick) { _nick = nick; }
void Client::set_password(std::string pass)  { _pass = pass; }

