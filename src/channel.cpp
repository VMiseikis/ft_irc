# include "channel.hpp"

Channel::~Channel()	{}

Channel::Channel(Server *server, Client *creator, std::string & name): _name(name), _server(server)	{
	_chops.push_back(creator);
}

std::string const	Channel::getName(void) const{
	return (_name);
}	

std::string	Channel::getTopic(void)	{
	return (_topic);
}

std::vector<Client * > & Channel::getChops(void)	{
	return (_chops);
}

std::vector<Client * > & Channel::getUsers(void)	{
	return (_users);
}

std::string	Channel::getNamesList(void)	{
	std::string ret = " :";
	std::vector<Client *>::iterator it;
	for (it = _users.begin(); it != _users.end(); it++)	{
		std::string	nick = (*it)->get_nick_name();
		if (isChanOp(*it))
			ret += "@";
		ret += (*it)->get_id().substr(1);
		if (it + 1 != _users.end())
			ret += " ";
	}
	return (ret + "\r\n");
}

void	Channel::names(Client *client)	{
	std::string	msg;
	msg = client->get_id() + " 353 " + client->get_nick_name();
	msg += " = " + _name + getNamesList();
	send(client->get_fd(), msg.c_str(), msg.length(), 0);
	msg = client->get_id() + " 366 " + client->get_nick_name();
	msg += " " + _name + " :End of /NAMES list\r\n";
	send(client->get_fd(), msg.c_str(), msg.length(), 0);
}

bool	Channel::isChanOp(Client *client) {
	if (getClientByNick(_chops, (*client).get_nick_name()))
		return (true);
	return (false);
}

bool	Channel::isOnChan(Client *client) {
	if (getClientByNick(_users, (*client).get_nick_name()))
		return (true);
	return (false);
}

void	Channel::newUser(Client	*client)	{
	if (getClientByNick(_users, client->get_nick_name()))
		return ;
	_users.push_back(client);
	client->join(this);
	std::string msg = client->get_id();
	msg += " JOIN :" + _name + "\r\n";
	broadcast(msg);
	topic(client);
	names(client);
}

void	Channel::broadcast(std::string msg)	{
	for (unsigned int i = 0; i < _users.size(); i++)	{
		send(_users[i]->get_fd(), msg.c_str(), msg.length(), 0);
	}
}

void	Channel::broadcast(Client *client, std::string msg)	{
	int fd = client->get_fd();
	for (unsigned int i = 0; i < _users.size(); i++)	{
		if (_users[i]->get_fd() == fd) 
			continue ;
		send(_users[i]->get_fd(), msg.c_str(), msg.length(), 0);
	}
}

void	Channel::part(Client *client)	{
	std::vector<Client *>::iterator	it;
	std::string	name;
	name = client->get_nick_name();
	for (it = _chops.begin(); it != _chops.end(); it++)	{
		if ((*it)->get_nick_name() == name)	{
			_chops.erase(it);
			break ;
		}
	}
	std::string msg = client->get_id();
	msg += " PART :" + _name + "\r\n";
	broadcast(msg);
	for (it = _users.begin(); it < _users.end(); it++)	{
			std::cout << (*it)->get_nick_name() << std::endl;
		if ((*it)->get_nick_name() == name)	{

			_users.erase(it);
			break ;
		}
	}
	if (_users.empty())	{
		return _server->delete_channel(this);
	}
	else	{
		for (unsigned int i = 0; i < _users.size(); i++)	{
			names(_users[i]);
		}
	}
}

void	Channel::dc(Client *client)	{
	std::vector<Client *>::iterator	it;
	std::string	name;
	name = client->get_nick_name();
	for (it = _chops.begin(); it != _chops.end(); it++)	{
		if ((*it)->get_nick_name() == name)	{
			_chops.erase(it);
			break ;
		}
	}
		for (it = _users.begin(); it < _users.end(); it++)	{
			if ((*it)->get_nick_name() == name)	{
				_users.erase(it);
				break ;
			}
	}
	if (_users.empty())	{
		return _server->delete_channel(this);
	}
	else	{
		std::string msg = client->get_id();
		msg += " QUIT :Client exited\r\n";
		broadcast(msg);
		for (unsigned int i = 0; i < _users.size(); i++)	{
			names(_users[i]);
		}
	}
}

void	Channel::topic(Client *client)	{
	if (_topic.empty())
		return client->reply(client->get_id(), " 331 " + client->get_nick_name() + " "+ _name + " :No topic");
	return client->reply(client->get_id(), " 332 " + client->get_nick_name() + " "+ _name + " :" + _topic);
}

void	Channel::topic(Client *client, std::string topic)	{
	if (topic[0] == ':')
		_topic = topic.substr(1);
	else
		_topic = topic;
	topic = " 332 " + client->get_nick_name() + " " + _name + " :" + _topic + "\r\n";
	for (unsigned int i = 0; i < _users.size(); i++)	{
		_users[i]->reply(_users[i]->get_id(), topic);
	}
}

