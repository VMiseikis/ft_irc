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
		ret += nick;
		if (it + 1 != _users.end())
			ret += " ";
	}
	std::cout << ret << " nameLISt\n";
	return (ret + "\r\n");
}

/*void	Channel::update(Client *user)	{
	msg = ":" + client->fullID() + " 353 " + client->get_nick_name();
	msg += " = " + _name + getNamesList();
	send(client->get_fd(), msg.c_str(), msg.length(), 0);
	msg = ":" + client->fullID() + " 366 " + client->get_nick_name();
	msg += " " + _name + " :End of NAMES list\r\n";
	send(client->get_fd(), msg.c_str(), msg.length(), 0);*/

/*bool	Channel::nameOk(std::string	name)	{
	if (name.length() > 50 || name.find_first_of(" ,") != std::string::npos)
		return (false)
	for (int i = 0; i < name.length(); i++)	{
		if (!std::isprint(name[i]))
			return (false)
	}
	return (true);
}*/
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
	std::string msg = ":" + client->fullID();
	msg += " JOIN :" + _name + "\r\n";
	std::cout << msg << std::endl;
	broadcast(msg);
	topic(client);
/*msg = ":" + client->fullID() + " 353 " + client->get_nick_name();
	msg += " = " + _name + getNamesList();
	send(client->get_fd(), msg.c_str(), msg.length(), 0);
	msg = ":" + client->fullID() + " 366 " + client->get_nick_name();
	msg += " " + _name + " :End of /NAMES list\r\n";
	send(client->get_fd(), msg.c_str(), msg.length(), 0);*/
	msg = " 353 " + client->get_nick_name() + " = " + _name + getNamesList();
	client->reply(msg);
	msg = " 366 " + client->get_nick_name() + " " + _name + " :End of /NAMES list\r\n";
	client->reply(msg);
}

void	Channel::broadcast(std::string msg)	{
	std::cout << "broadcast visiems kanale\n";
	for (unsigned int i = 0; i < _users.size(); i++)	{
		send(_users[i]->get_fd(), msg.c_str(), msg.length(), 0);
	}
}
/*void	Channel::broadcast(std::string msg)	{
	std::cout << "broadcast visiems kanale\n";
	for (unsigned int i = 0; i < _users.size(); i++)	{
		_users[i]->reply(msg);
	}
}*/

void	Channel::broadcast(Client *client, std::string msg)	{
	std::cout << "broadcast\n";
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
//
//			std::cout << (*it)->get_nick_name() << std::endl;
		if ((*it)->get_nick_name() == name)	{
			_chops.erase(it);
			break ;
		}
	}
	std::string msg = ":" + client->fullID();
	msg += " PART :" + _name + "\r\n";
	std::cout << msg << std::endl;
	broadcast(msg);
	for (it = _users.begin(); it < _users.end(); it++)	{
			std::cout << (*it)->get_nick_name() << std::endl;
		if ((*it)->get_nick_name() == name)	{

			_users.erase(it);
			break ;
		}
	}
	if (_users.empty())	{
		return delete_channel(this);
		//return _server->deleteChannel(this);
	}
	else	{
		if (_chops.empty())
			_chops.push_back(*(_users.begin()));
//		std::string	msg;
		for (unsigned int i = 0; i < _users.size(); i++)	{
			_users[i]->reply(" 353 " + _users[i]->get_nick_name() + " = " + _name + getNamesList());
			_users[i]->reply(" 366 " + _users[i]->get_nick_name() + " " + _name + " :End of /NAMES list\r\n");
		}
	}
}

void Channel::delete_channel(Channel *channel)
{
	for (std::vector<Channel *>::iterator it = _server->get_channels().begin(); it != _server->get_channels().end(); ++it)
	{
		if (*it == channel)
		{
			_server->get_channels().erase(it);
			break ;
		}
	}
}


void	Channel::dc(Client *client)	{
	std::vector<Client *>::iterator	it;
	std::string	name;
	name = client->get_nick_name();
	for (it = _chops.begin(); it != _chops.end(); it++)	{

//std::cout << _name << " chOP " << std::endl;
//			std::cout << (*it)->get_nick_name() << std::endl;
		if ((*it)->get_nick_name() == name)	{
			_chops.erase(it);
			break ;
		}
	}
		for (it = _users.begin(); it < _users.end(); it++)	{
//std::cout << _name << " user " << std::endl;
//			std::cout << (*it)->get_nick_name() << std::endl;
		if ((*it)->get_nick_name() == name)	{

			_users.erase(it);
			break ;
		}
	}
	if (_users.empty())	{
		return delete_channel(this);
		//return _server->deleteChannel(this);

	}
	else	{
		std::string msg = ":" + client->fullID();
		msg += " PART :" + _name + "\r\n";
		std::cout << msg << std::endl;
		broadcast(msg);
		if (_chops.empty())
			_chops.push_back(*(_users.begin()));
		for (unsigned int i = 0; i < _users.size(); i++)	{
			_users[i]->reply(" 353 " + _users[i]->get_nick_name() + " = " + _name + getNamesList());
			_users[i]->reply(" 366 " + _users[i]->get_nick_name() + " " + _name + " :End of /NAMES list\r\n");
		}
	}
}

void	Channel::topic(Client *client)	{
	if (_topic.empty())
		return client->reply(" 331 " + client->get_nick_name() + " "+ _name + " :No topic\r\n");
	return client->reply(" 332 " + client->get_nick_name() + " "+ _name + " :" + _topic + "\r\n");
}

void	Channel::topic(Client *client, std::string topic)	{
	if (topic[0] == ':')
		_topic = topic.substr(1);
	else
		_topic = topic;
	topic = " 332 " + client->get_nick_name() + " " + _name + " :" + _topic + "\r\n";
	for (unsigned int i = 0; i < _users.size(); i++)	{
		_users[i]->reply(topic);
	}
}

