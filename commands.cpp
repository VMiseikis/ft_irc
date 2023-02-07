#include "commands.hpp"

Commands::Commands(Server *server) : _server(server)
{
	_commands.insert(std::make_pair("WHO", &Commands::who_command));
	_commands.insert(std::make_pair("PASS", &Commands::pass_command));
	_commands.insert(std::make_pair("USER", &Commands::user_command));
	_commands.insert(std::make_pair("NICK", &Commands::nick_command));
	_commands.insert(std::make_pair("OPER", &Commands::oper_command));
	_commands.insert(std::make_pair("ISON", &Commands::ison_command));
	_commands.insert(std::make_pair("PING", &Commands::pong_command));
	_commands.insert(std::make_pair("MODE", &Commands::mode_command));
	_commands.insert(std::make_pair("KICK", &Commands::kick_command));
	_commands.insert(std::make_pair("KILL", &Commands::kill_command));
	_commands.insert(std::make_pair("LIST", &Commands::list_command));
	_commands.insert(std::make_pair("PART", &Commands::part_command));
	_commands.insert(std::make_pair("JOIN", &Commands::join_command));
	_commands.insert(std::make_pair("QUIT", &Commands::quit_command));
	_commands.insert(std::make_pair("WALL", &Commands::wall_command));
	_commands.insert(std::make_pair("TOPIC", &Commands::tpic_command));
	_commands.insert(std::make_pair("SQUIT", &Commands::squi_command));
	_commands.insert(std::make_pair("NOTICE", &Commands::pmsg_command));
	_commands.insert(std::make_pair("PRIVMSG", &Commands::pmsg_command));
}

Commands::~Commands() { _commands.clear(); }

std::string Commands::responce_msg(int err, std::string client, std::string arg)
{
	switch (err)
	{
		case RPL_UMODEIS:
			return (" 221 " + client);
		case RPL_ISON:
			return (" 303 " + client);
		case RPL_ENDOFWHO:
			return (" 315 :" + client + " " + arg + " :End of WHO list");
		case RPL_LISTSTART:
			return (" 321 " + client + " Channels :Users Name");
		case RPL_LIST:
			return (" 322 " + client + " " + arg);
		case RPL_LISTEND:
			return (" 323 " + client + " :End of LIST");
		case RPL_WHOREPLY:
			return (" 352 :" + client );
		case RPL_NAMREPLY:
			return (" 353 " + client + " " + arg);
		case RPL_WHOSPCRPL:		
			return (" 354 " + client + " " + arg);
		case RPL_ENDOFNAMES:
			return (" 366 " + client + arg);	
		case RPL_KILLDONE:
			return (" 361 " + client + " " + arg);
		case RPL_YOUREOPER:
			return (" 381 " + client + " :You are now an IRC operator");
		case ERR_NOSUCHNICK:
			return (" 401 " + client + " " + arg + " :No such nick");
		case ERR_NOSUCHCHANNEL:
			return (" 403 " + client + " " + arg + " :No such channel");	
		case ERR_CANNOTSENDTOCHAN:
			return (" 404 " + client + " :Cannot send to channel, client isn't in the channel");
		case ERR_NORECIPIENT:
			return (" 411 " + client + " :No recipient given(PRIVMSG)");
		case ERR_NOTEXTTOSEND:
			return (" 412 " + client + " :No text to send");
		case ERR_UNKNOWNCOMMAND:
			return (" 421 " + client + " " + arg + " :Unknown command");
		case ERR_NONICKNAMEGIVEN:
			return (" 431 " + client + " " + arg + " :No nickname give to change to");
		case ERR_ERRONEUSNICKNAME:
			return (" 432 " + client + " " + arg + " :Erroneus nickname");
		case ERR_NICKNAMEINUSE:
			return (" 433 " + client + " " + arg + " :Nickname is already in use");
		case ERR_NOTONCHANNEL:
			return (" 442 " + client + " " + arg + " :You or target are not on that channel");		
		case ERR_NOTREGISTERED:
			return (" 451 " + client + " :You have not registered");
		case ERR_NEEDMOREPARAMS:
			return (" 461 " + client + " " + arg + " :Not enough, or to many parameters");
		case ERR_ALREADYREGISTRED:
			return (" 462 " + client + " :Unauthorized command");
		case ERR_PASSWDMISMATCH:
			return (" 464 " + client + " :Password incorrect");
		case ERR_NOPRIVILEGES:
			return (" 481 " + client + " :Permission Denied - You're not an IRC operator");
		case ERR_CHANOPRIVSNEEDED:
			return (" 482 " + client + " " + arg + " :You're not channel operator");
		case ERR_UMODEUNKNOWNFLAG:
			return (" 501 " + client + " :Unknown MODE flag");	
		case ERR_NOCHANELNAME:
			return (client + ":This command can only be used with a channel");
		default:
			return "";
	}
}

void Commands::get_arguments(std::string line, std::vector<std::string> *args)
{
	(*args).clear();

	for (size_t i = 0; i < line.length(); )
	{
		i = line.find_first_not_of(WHITESPACES , i);
		if (i == std::string::npos)
			return ;
		if (line[i] == ':')
			return (*args).push_back(line.substr(i, line.size()));
		if (line[i] == '"')
			(*args).push_back(line.substr(i, line.find_first_of('"', i + 1) - i + 1));
		else
			(*args).push_back(line.substr(i, line.find_first_of(WHITESPACES, i) - i));
		i += (*args).back().length();
	}
}

void Commands::execute_command(Client *client, std::string line)
{
	size_t i;
	std::string args = "";
	std::string cmd = line.substr(0, line.find_first_of(WHITESPACES));
	
	for (i = 0; cmd[i]; i++)
		cmd[i] = towupper(cmd[i]);

	if (cmd == "CAP")
		return ;

	i = line.find_first_not_of(WHITESPACES, cmd.length());
	if (i != std::string::npos)
		args = line.substr(i, line.length());

	if (cmd != "PASS" && client->is_new())
		return client->reply(client->get_id(), responce_msg(ERR_NOTREGISTERED, client->get_nick_name(), ""));

	try {
		(this->*_commands.at(cmd))(client, cmd, args);
	} catch (const std::out_of_range &err) { 
		return client->reply(client->get_id(), responce_msg(ERR_UNKNOWNCOMMAND, client->get_nick_name(), cmd));
	}
}

void Commands::pass_command(Client *client, std::string cmd, std::string arg)
{
	if (arg.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

	if (!client->is_new())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));
		
	if (arg[0] == ':')
		arg = arg.substr(1, arg.length());

	if (_server->get_password() != arg)
		return client->reply(client->get_id(), responce_msg(ERR_PASSWDMISMATCH, client->get_nick_name(), ""));

	std::cout << "Client " << client->get_hostname() << " loged to the server\n";
	client->set_status(client->get_status() + 1);	
}

void Commands::user_command(Client *client, std::string cmd, std::string line)
{
	std::vector<std::string> args;

	if (!client->is_auth() || !client->get_user_name().empty())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	get_arguments(line, &args);

	if (args.size() != 4)
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

	if (args[3][0] == ':')
	{
		size_t i = args[3].find_first_not_of(WHITESPACES, 1);
		if (i != std::string::npos)
			client->set_real_name((args[3].substr(i, args[3].size())));
		else
			return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));
	}
	else
		client->set_real_name(args[3]);
	
	client->set_user_name(args[0]);

	std::cout << "Client " << client->get_hostname() << " set his USER name to: " << client->get_user_name() << " and REAL name to: " << client->get_real_name() << std::endl;

	client->welcome();
}

void Commands::nick_command(Client *client, std::string cmd, std::string line)
{
	std::vector<std::string> args;

	if (client->is_new())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), cmd));

	std::string nick = line.substr(0, line.find_first_of(WHITESPACES));

	if (nick.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NONICKNAMEGIVEN, client->get_nick_name(), cmd));

	if (nick[0] == '@')
		return client->reply(client->get_id(), responce_msg(ERR_ERRONEUSNICKNAME, client->get_nick_name(), nick));

	if (client->get_nick_name() != nick)
	{
		if (_server->get_client(nick))
			return client->reply(client->get_id(), responce_msg(ERR_NICKNAMEINUSE, client->get_nick_name(), nick));

		_server->wall(client->get_id() + " NICK :" + nick + "\r\n");
		client->set_nick_name(nick);
		std::cout << "Client " << client->get_id() << " set his NICK name to " << client->get_nick_name() << std::endl;
		if (client->get_status() < 2)
			client->welcome();
	}
}

void Commands::join_command(Client *creator, std::string cmd, std::string args)
{
	(void) cmd;

	if (creator->get_status() < REGISTERED)
		return creator->reply(creator->get_id(), responce_msg(ERR_NOTREGISTERED, creator->get_nick_name(), ""));

	std::vector<std::string> names;
	if (args.empty())
		return creator->reply(creator->get_id(), responce_msg(ERR_NEEDMOREPARAMS, creator->get_nick_name(), ""));

	size_t i = args.find(',', 0);
	if (i == std::string::npos)
		names.push_back(args);
	else {
		while (i != std::string::npos)
		{
			names.push_back(args.substr(0, i));
			args = args.substr(i + 1);
			if (args[0] != '#')
				break ;
			i = args.find(',', 0);
			if (i == std::string::npos)
				names.push_back(args);
		}
	}

	for (unsigned int i = 0; i < names.size(); i++) {
		if (!i)	{
			if (names[i][i] != '#')
				names[i] = "#" + names[i];
		}
		Channel	*exists = _server->get_channel(names[i]);
		if (exists)
		{
			(*exists).newUser(creator);
			std::cout << "Client " << creator->get_id() << " joined a channel " << exists->getName() << std::endl;
		}
		else {
			_server->get_channels().push_back(new Channel(_server, creator, names[i]));
			_server->get_channels().back()->newUser(creator);
			std::cout << "Client " << creator->get_id() << " created a channel " << _server->get_channels().back()->getName() << std::endl;
		}
	}
}

void Commands::pmsg_command(Client *client, std::string cmd, std::string line)
{
	size_t i;
	std::string	msg = "";
	std::string send_msg;

	if (!client->is_registered() && !client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	std::string nick = line.substr(0, line.find_first_of(WHITESPACES));

	if (nick.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NORECIPIENT, client->get_nick_name(), ""));

	i = line.find_first_not_of(WHITESPACES, nick.size());
	if (i != std::string::npos)
		msg = line.substr(i, line.size());
	if (msg.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NOTEXTTOSEND, client->get_nick_name(), ""));

	if (nick[0] != '#')	{
		Client *receiver = _server->get_client(nick);
		if (!receiver)
			return client->reply(client->get_id(), responce_msg(ERR_NOSUCHNICK, client->get_nick_name(), nick));

		if (msg[0] != ':')
			send_msg = " " + cmd + " " + nick + " :" + msg;
		else
			send_msg = " " + cmd + " " + nick + " " + msg;

		std::cout << "Client " << client->get_id() << " has send private message to " << receiver->get_id() << std::endl;
		return receiver->reply(client->get_id(), send_msg);
	} else {
		Channel	*channel;
		channel = _server->get_channel(nick);
		if (!channel)
			return client->reply(client->get_id(), responce_msg(ERR_NOSUCHCHANNEL, client->get_nick_name(), nick));

		if (!getClientByNick(channel->getUsers(), client->get_nick_name()))
			return client->reply(client->get_id(), responce_msg(ERR_CANNOTSENDTOCHAN, client->get_nick_name(), ""));

		msg = client->get_id() + " " + cmd + " " + line + "\r\n";
		std::cout << "Client " << client->get_id() << " has wrote message in the channel " << channel->getName() << std::endl;
		channel->broadcast(client, msg);
	}
}

void Commands::oper_command(Client *client, std::string cmd, std::string line)
{
	size_t i;

	if (client->is_admin())
		return;

	if (!client->is_registered())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	std::string name = line.substr(0, line.find_first_of(WHITESPACES));
	if (name.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

	i = line.find_first_not_of(WHITESPACES , name.size());
	if (i == std::string::npos)
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

	std::string pass = line.substr(i, line.find_first_of(WHITESPACES, i) - i);

	if (_server->get_admin_name() != name || _server->get_admin_pass() != pass)
		return client->reply(client->get_id(), responce_msg(ERR_PASSWDMISMATCH, client->get_nick_name(),""));

	client->set_status(client->get_status() + 1);

	std::cout << "Client " << client->get_id() << " activated his ADMIN powers " << std::endl;
	return client->reply(client->get_id(), responce_msg(RPL_YOUREOPER, client->get_nick_name(), ""));
}

void Commands::ison_command(Client *client, std::string cmd, std::string line)
{
	std::string msg = "";
	std::vector<std::string> args;

	if (!client->is_registered())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	get_arguments(line, &args);

	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it)
		if (_server->get_client(*it))
			msg += *it + " ";

	return client->reply(client->get_id(), responce_msg(RPL_ISON, msg, cmd));
}


void Commands::pong_command(Client *client, std::string cmd, std::string line)
{
	(void) cmd;

	if (!client->is_registered() && !client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	return client->reply(client->get_id(), " PONG :" + line + "\r\n");
}

void Commands::part_command(Client *client, std::string cmd, std::string args)	{
	(void) cmd;
	if (client->get_status() < REGISTERED)
		return client->reply(client->get_id(), responce_msg(ERR_NOTREGISTERED, client->get_nick_name(), ""));

	if (args.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), args));

	Channel	*exists;
	args = args.substr(0, args.find_first_of(WHITESPACES));
	exists = _server->get_channel(args);
	if (!exists)
		return client->reply(client->get_id(), responce_msg(ERR_NOSUCHCHANNEL, client->get_nick_name(), args));

	if (!(*exists).isOnChan(client))
		return client->reply(client->get_id(), responce_msg(ERR_NOTONCHANNEL, client->get_nick_name(), args));

	std::cout << "Client " << client->get_id() << " left the channel " << exists->getName() << std::endl;
	client->part(exists);
}

void Commands::tpic_command(Client *client, std::string cmd, std::string args)
{
	(void)cmd;

	size_t i;
	std::string	msg;
	if (client->get_status() < REGISTERED)
		return client->reply(client->get_id(), responce_msg(ERR_NOTREGISTERED, client->get_nick_name(), ""));

	if (args.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), args));

	std::string name = args.substr(0, args.find_first_of(WHITESPACES));
	Channel	*channel;
	channel = _server->get_channel(name);
	if (!channel)
		return client->reply(client->get_id(), responce_msg(ERR_NOSUCHCHANNEL, client->get_nick_name(), name));

	if (!getClientByNick(channel->getUsers(), client->get_nick_name()))
		return client->reply(client->get_id(), responce_msg(ERR_CANNOTSENDTOCHAN, client->get_nick_name(), ""));

	i = args.find_first_not_of(WHITESPACES, name.size());
	if (i != std::string::npos)
		msg = args.substr(i, args.size());
	if (msg.empty())
		channel->topic(client);
	else
		channel->topic(client, msg);
	
	std::cout << "Client " << client->get_id() << " changed channel " << channel->getName() << " topic to: " << channel->getTopic() << std::endl;
}

static std::string uitos(unsigned int i)	{
	char s[64];
	char c;
	if (i == 0)	{
		s[0] = '0';
		s[1] = '\0';
		return (s);
	}
	unsigned int j = 0;
	while (i)	{
		s[j] = i % 10 + '0';
		i /= 10;
		j++;
	}
	s[j] = '\0';
	j--;
	while (i < j)	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
		i++;
		j--;
	}
	return (s);
}
	
void Commands::list_command(Client *client, std::string cmd, std::string args)
{
	(void)cmd;
	(void)args;
	if (client->get_status() < REGISTERED)
		return client->reply(client->get_id(), responce_msg(ERR_NOTREGISTERED, client->get_nick_name(), ""));

	if (_server->get_channels().empty())
		return client->reply(client->get_id(), responce_msg(RPL_LISTEND, client->get_nick_name(), ""));

	client->reply(client->get_id(), responce_msg(RPL_LISTSTART, client->get_nick_name(), ""));

	for (std::vector<Channel *>::iterator it = _server->get_channels().begin(); it != _server->get_channels().end(); it++)
		client->reply(client->get_id(), responce_msg(RPL_LIST, client->get_nick_name(), (*it)->getName() + " " + uitos((*it)->getUsers().size()) + " :" + (*it)->getTopic()));

	std::cout << "Client " << client->get_id() << " called LIST command " << std::endl;
	return client->reply(client->get_id(), responce_msg(RPL_LISTEND, client->get_nick_name(), ""));
}

void Commands::mode_command(Client *client, std::string cmd, std::string line)
{
	std::vector<std::string> args;

	if (!client->is_registered() && !client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	get_arguments(line, &args);

	if (args.size() > 0 && args[0][0] =='#')
	{
		if (args.size() == 1)
			return client->reply(client->get_id(), responce_msg(RPL_UMODEIS, client->get_nick_name(), ""));

		if (args.size() < 3)
			return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

		Channel *channel = _server->get_channel(args[0]);
		if (!channel)
			return client->reply(client->get_id(), responce_msg(ERR_NOSUCHCHANNEL, client->get_nick_name(), args[0]));

		Client *target = _server->get_client(args[2]);
		if (!target)
			return client->reply(client->get_id(), responce_msg(ERR_NOSUCHNICK, client->get_nick_name(), args[2]));

		if (!channel->isOnChan(client) || !channel->isOnChan(target))
			return client->reply(client->get_id(), responce_msg(ERR_NOTONCHANNEL, client->get_nick_name(), args[0]));

		if (!channel->isChanOp(client) && !client->is_admin())
			return client->reply(client->get_id(), responce_msg(ERR_CHANOPRIVSNEEDED, client->get_nick_name(), args[0]));

		char plusminus = '\0';
		std::string mode = "";
		for (int i = 0; args[1][i]; i++)
		{
			if (iswalpha(args[1][i]) && mode.find_first_of(args[1][i]) == std::string::npos)
				mode.push_back(args[1][i]);
			else if (plusminus == '\0' && (args[1][i] == '-' || args[1][i] == '+'))
				plusminus = args[1][i];
		}
		if (plusminus == '\0')
			plusminus = '+';
		for (int i = 0; mode[i]; i++)
		{
			switch (mode[i])
			{
				case 'o':
				{
					if (plusminus == '-' && channel->isChanOp(target))
					{
						for (std::vector<Client *>::iterator it = channel->getChops().begin(); it != channel->getChops().end(); ++it)
							if ((*it)->get_nick_name() == args[2])
							{
								channel->getChops().erase(it);
								break ;
							}
					}
					else if (plusminus == '+' && !channel->isChanOp(target))
						channel->getChops().push_back(target);
					for (std::vector<Client *>::iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it)
						(*it)->reply((*it)->get_id(), " MODE " + args[0] + " " + plusminus + mode[i] + " " + target->get_nick_name());
					std::cout << "Channel " << channel->getName() << " operator set mode: " << plusminus << mode[i] << " to user " << target->get_id() << std::endl;
					break ;
				}
				default:
					return client->reply(client->get_id(), responce_msg(ERR_UMODEUNKNOWNFLAG, client->get_nick_name(), ""));
			}
		}
	}
}

void Commands::who_command(Client *client, std::string cmd, std::string line)
{
	(void) cmd;

	if (!client->is_registered())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	std::string name = line.substr(0, line.find_first_of(WHITESPACES));
	if (name.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

	if (name[0] == '#')
	{
		Channel *channel = _server->get_channel(name);
		if (!channel)
			return client->reply(client->get_id(), responce_msg(ERR_NOSUCHCHANNEL, client->get_nick_name(), name));

		for (std::vector<Client *>::iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it)
			client->reply(client->get_id(), responce_msg(RPL_WHOSPCRPL, client->get_nick_name(), " " + name + " " + (*it)->get_user_name() + " " + (*it)->get_hostname() + " " + _server->get_name() + " " + (*it)->get_nick_name() + " H 0 " + (*it)->get_real_name()));

		return client->reply(client->get_id(), responce_msg(RPL_ENDOFWHO, client->get_nick_name(), name));
	}

	Client *target = _server->get_client(name);
	if (!target)
		return client->reply(client->get_id(), responce_msg(ERR_NOSUCHNICK, client->get_nick_name(), name));

	if (target->get_channels().empty())
		return client->reply(client->get_id(), responce_msg(RPL_WHOSPCRPL, client->get_nick_name(), " * " + target->get_user_name() + " " + target->get_hostname() + " " + _server->get_name() + " " + target->get_nick_name() + " H 0 " + target->get_real_name()));
	else
		return client->reply(client->get_id(), responce_msg(RPL_WHOSPCRPL, client->get_nick_name(), " " + target->get_channels()[0]->getName() + " " + target->get_user_name() + " " + target->get_hostname() + " " + _server->get_name() + " " + target->get_nick_name() + " H 0 " + target->get_real_name()));

	return client->reply(client->get_id(), responce_msg(RPL_ENDOFWHO, client->get_nick_name(), name));
}

void Commands::kick_command(Client *client, std::string cmd, std::string line)
{
	std::vector<std::string> args;

	if (!client->is_registered() && !client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_ALREADYREGISTRED, client->get_nick_name(), ""));

	get_arguments(line, &args);

	if (args.size() < 2)
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

	if (args[0][0] != '#')
		return client->reply(client->get_id(), responce_msg(ERR_NOCHANELNAME, client->get_nick_name(), ""));

	Channel *channel = _server->get_channel(args[0]);
	if (!channel)
		return client->reply(client->get_id(), responce_msg(ERR_NOSUCHCHANNEL, client->get_nick_name(), args[0]));

	if (!channel->isChanOp(client) && !client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_CHANOPRIVSNEEDED, client->get_nick_name(), args[0]));

	Client *target = _server->get_client(args[1]);
	if (!target)
		return client->reply(client->get_id(), responce_msg(ERR_NOSUCHNICK, client->get_nick_name(), args[1]));

	for (std::vector<Client *>::iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it)
	{
		if (args.size() == 3 && !args[2].empty())
			(*it)->reply((*it)->get_id() , " KICK " + args[0] + " " + args[1] + " " + args[2]);
		else
			(*it)->reply((*it)->get_id() , " KICK " + args[0] + " " + args[1] + " :Your behavior is not conducive to the desired environment");
	}

	std::cout << "Client " << target->get_id() << " was kicked out of the channel: " << channel->getName() << " by operator: " << client->get_id() << std::endl;

	for (std::vector<Client *>::iterator it = channel->getChops().begin(); it != channel->getChops().end(); ++it)
	{
		if ((*it)->get_nick_name() == args[1])
		{
			channel->getChops().erase(it);
			break;
		}
	}
	for (std::vector<Client *>::iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it)
	{
		if ((*it)->get_nick_name() == args[1])
		{
			channel->getUsers().erase(it);
			break;
		}
	}
}

void Commands::quit_command(Client *client, std::string cmd, std::string args)
{
	(void)args;

	client->reply(client->get_id(), " " + cmd + " " + client->get_nick_name() +  " :Bye Irc");
	_server->client_quit(client->get_fd());
}

void Commands::kill_command(Client *client, std::string cmd, std::string line)
{
	if (!client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_NOPRIVILEGES, client->get_nick_name(), ""));
	
	std::string nick = line.substr(0, line.find_first_of(WHITESPACES));

	if (nick.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));

	Client *target = _server->get_client(nick);
	if (!target)
		return client->reply(client->get_id(), responce_msg(ERR_NOSUCHNICK, client->get_nick_name(), nick));

	target->reply(target->get_id(), responce_msg(RPL_KILLDONE, "", nick + " :Your behavior is not conducive to the desired environment"));
	std::cout << "Client " << target->get_id() << " connection was terminated by admin: " << client->get_id() << std::endl;
	quit_command(target, "QUIT", "");
}

void Commands::squi_command(Client *client, std::string cmd, std::string args)
{
	(void) cmd;
	(void)args;

	if (!client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_NOPRIVILEGES, client->get_nick_name(), ""));

	_server->broadcast_to_all_clients("Server is going down now.");
	std::cout << "Admin " << client->get_id() << " turned server off" << std::endl;
	Server::turn_off(1);
}

void Commands::wall_command(Client *client, std::string cmd, std::string args)
{
	if (!client->is_admin())
		return client->reply(client->get_id(), responce_msg(ERR_NOPRIVILEGES, client->get_nick_name(), ""));

	if (args.empty())
		return client->reply(client->get_id(), responce_msg(ERR_NEEDMOREPARAMS, client->get_nick_name(), cmd));
	std::cout << "Admin " << client->get_id() << " has send announcement to everyone" << std::endl;
	_server->broadcast_to_all_clients(args);
}
