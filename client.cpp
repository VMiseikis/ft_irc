#include "client.hpp"


Client::Client(int fd, std::string ip, int port) : _fd(fd), _port(port), _status(NEW), _ip(ip), _user(), _real(), _nick(), _pass() {}

Client::~Client() {}

int Client::get_fd()	{ return _fd; }
int Client::get_port()	{ return _port; }
int Client::get_status() { return _status; }
std::string Client::get_ip() 		{ return _ip; }
std::string Client::get_user_name() { return _user; }
std::string Client::get_real_name() { return _real; }
std::string Client::get_nick_name() { return _nick; }
std::string Client::get_password()	{ return _pass; }

void Client::set_status(int status) 			{ _status = status; }
void Client::set_user_name(std::string user) 	{ _user = user; }
void Client::set_real_name(std::string real)	{ _real = real; }
void Client::set_nick_name(std::string nick) 	{ _nick = nick; }
void Client::set_password(std::string pass)  	{ _pass = pass; }

bool Client::is_new() 			{ return (_status == NEW); }
bool Client::is_auth() 			{ return (_status == HANDSHAKE); }
bool Client::is_registered() 	{ return (_status == REGISTERED); }


void Client::reply(const std::string &msg)
{
	std::string reply;

	reply = _nick;
	if (!_user.empty())
		reply += "!" + _user;
	
	if (!_ip.empty())
		reply += "@" + _ip;

	// if (_port != 0)
	// 	reply += ":" + [_port];

	reply.append(msg);
	send(_fd, reply.c_str(), reply.length(), 0);
}

Client	*getClientByNick(std::vector<Client *> &from, std::string nick)	{
	for (std::vector<Client *>::iterator it = from.begin(); it != from.end(); it++)	{
		if ((*it)->get_nick_name() == nick)
			return (*it);
	}
	return(NULL);
}
