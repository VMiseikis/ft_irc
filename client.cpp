#include "client.hpp"

Client::Client(int fd, std::string ip, int port, std::string hostname) : _fd(fd), _port(port), _status(NEW), _ip(ip), _user(), _real(), _nick(), _pass(), _hostname(hostname) {}

Client::~Client() { close(_fd); }

int Client::get_fd()	{ return _fd; }
int Client::get_port()	{ return _port; }
int Client::get_status() { return _status; }
std::string Client::get_ip() 		{ return _ip; }
std::string Client::get_user_name() { return _user; }
std::string Client::get_real_name() { return _real; }
std::string Client::get_nick_name() { return _nick; }
std::string Client::get_password()	{ return _pass; }
std::string Client::get_hostname()	{ return _hostname; }
std::vector<Channel *> Client::get_channels() { return _channels; }

void Client::set_status(int status) 			{ _status = status; }
void Client::set_user_name(std::string user) 	{ _user = user; }
void Client::set_real_name(std::string real)	{ _real = real; }
void Client::set_nick_name(std::string nick) 	{ _nick = nick; }
void Client::set_password(std::string pass)  	{ _pass = pass; }
void Client::set_hostname(std::string host)  	{ _hostname = host; }

bool Client::is_new() 			{ return (_status == NEW); }
bool Client::is_auth() 			{ return (_status == HANDSHAKE); }
bool Client::is_registered() 	{ return (_status == REGISTERED); }
bool Client::is_admin() 		{ return (_status == ADMIN); }


void Client::welcome()
{
	if (_status == HANDSHAKE && !_user.empty() && !_real.empty() && !_nick.empty())
	{
		std::string welcome_message = ":MultiplayerNotepad 001 " + _nick + " :Welcome to MultiplayerNodepad " + _nick + "\r\n";
		// reply(" 001 " + _nick + " :Welcome to MultiplayerNodepad " + _nick + "\r\n");	
		send(_fd, welcome_message.c_str(), welcome_message.length(), 0);
		_status++;
		std::cout << "Issiustas welcome" << "\n";
	}
}

void Client::reply(const std::string &msg)
{
	std::string reply = ":"; ///??????

	reply += _nick;
	if (!_user.empty())
		reply += "!" + _user;
	
	if (!_ip.empty())
		reply += "@" + _ip;

	// if (_port != 0)
	// 	reply += ":" + [_port];

	reply.append(msg);

	std::cout << reply << std::endl;
	send(_fd, reply.c_str(), reply.length(), 0);
}

std::string	Client::fullID(void)	{
	std::string	id;

	id = _nick;
	if (!_user.empty())
		id += "!" + _user;
	if (!_ip.empty())
		id += "@" + _ip;
	return (id);
}

std::string	Client::sendMsg(std::string msg)
{
	std::string reply = ":";

	reply += _nick;
	if (!_user.empty())
		reply += "!" + _user;
	
	if (!_ip.empty())
		reply += "@" + _ip;

	reply.append(msg);
	return (reply + "\r\n");
}

Client	*getClientByNick(std::vector<Client *> &from, std::string nick)	{
	for (std::vector<Client *>::iterator it = from.begin(); it != from.end(); it++)	{
		if ((*it)->get_nick_name() == nick)
			return (*it);
	}
	return(NULL);
}
void	Client::join(Channel *channel)	{
	_channels.push_back(channel);
}

void	Client::part(Channel *channel)	{
	channel->part(this);
	std::vector<Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); it++)	{
		if (*it == channel)	{
			_channels.erase(it);
			break ;
		}
	}
}
void	Client::dc(void)	{
	std::vector<Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); it++)	{
		(*it)->dc(this);
	}
	_channels.clear();
}		
