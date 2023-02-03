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

std::string responce_msg(std::string client, int err, std::string arg)
{
	switch (err)
	{
		// kolkas nznau kaip atskirti ar tai komanda ar tik kliento message
		// case ERR_UNKNOWNCOMMAND:
		// 	return (" 421 " arg + " :Unknown command.\r\n");

		case RPL_UMODEIS:
			return (" 221 " + client + " \r\n");
		case RPL_ISON:
			return (" 303 " + client + "\r\n");
		case RPL_ENDOFWHO:
			return (" 315 :" + client + " " + arg + " :End of WHO list\r\n");
		//"<client> <channel> <username> <host> <server> <nick> <flags> :<hopcount> <realname>"
		case RPL_WHOREPLY:
			return (" 352 :" + client + "\r\n");
		case RPL_NAMREPLY:
			return (" 353 " + client + " " + arg + "\r\n");
		case RPL_WHOSPCRPL:		
			return (" 354 " + client + " " + arg + "\r\n");
		case RPL_ENDOFNAMES:
			return (" 366 " + client + arg + "\r\n");
		case ERR_NOSUCHNICK:
			return (" 401 " + client + " " + arg + ":No such nick/channel\r\n");
		case ERR_NOSUCHCHANNEL:
			return (" 403 " + client + " " + arg + ":No such channel\r\n");		
		case RPL_KILLDONE:
			return (" 361 " + client + " " + arg + "\r\n");
		case RPL_YOUREOPER:
			return (" 381 " + client + " :You are now an IRC operator.\r\n");
		case ERR_NONICKNAMEGIVEN:
			return (" 431 " + client + " " + arg + " :No nickname give to change to.\r\n");
		case ERR_ERRONEUSNICKNAME:
			return (" 432 " + client + " " + arg + " :Erroneus nickname.\r\n");
		case ERR_NICKNAMEINUSE:
			return (" 433 " + client + " " + arg + " :Nickname is already in use.\r\n");
		case ERR_NOTONCHANNEL:
			return (" 442 " + client + " " + arg + " :You or target are not on that channel.\r\n");		
		case ERR_NOTREGISTERED:
			return (" 451 " + client + " :You have not registered.\r\n");
		case ERR_NEEDMOREPARAMS:
			return (" 461 " + client + " " + arg + " :Not enough, or to many parameters\r\n");
		case ERR_ALREADYREGISTRED:
			return (" 462 " + client + " :Unauthorized command\r\n");
		case ERR_PASSWDMISMATCH:
			return (" 464 " + client + " :Password incorrect\r\n");
		case ERR_NOPRIVILEGES:
			return (" 481 " + client + " :Permission Denied - You're not an IRC operator\r\n");
		case ERR_CHANOPRIVSNEEDED:
			return (" 482 " + client + " " + arg + " :You're not channel operator\r\n");
		case ERR_UMODEUNKNOWNFLAG:
			return (" 501 " + client + " :Unknown MODE flag\r\n");	
		case ERR_NOCHANELNAME:
			return (client + ":This command can only be used with a channel\r\n");
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
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOTREGISTERED, ""));

	try {
		(this->*_commands.at(cmd))(client, cmd, args);
	} catch (const std::out_of_range &err) { 
		//jeigu tokios komandos neradome, reiskia, kad tai tik paprasta 
		// zinute, todel turim jabroadcastinti i kanala ar kazkas panasaus
		// broadcast message or handle different way, idk
	}
}

void Commands::pass_command(Client *client, std::string cmd, std::string arg)
{
	if (arg.empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	if (!client->is_new())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));
		
	if (_server->get_password() != arg)
		return client->reply(responce_msg(client->get_nick_name(), ERR_PASSWDMISMATCH, ""));

	std::cout << "PASS sutampa su:" << arg << "\n";
	client->set_status(client->get_status() + 1);	
}

void Commands::user_command(Client *client, std::string cmd, std::string line)
{
	std::vector<std::string> args;

	if (!client->is_auth() || !client->get_user_name().empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	get_arguments(line, &args);

	if (args.size() != 4)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	if (args[3][0] == ':')
	{
		size_t i = args[3].find_first_not_of(WHITESPACES, 1);
		if (i != std::string::npos)
			client->set_real_name((args[3].substr(i, args[3].size())));
		else
			return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));
	}
	else
		client->set_real_name(args[3]);
	
	client->set_user_name(args[0]);
	std::cout << "USER name pakeistas i:" << client->get_user_name() << "\n";

	std::cout << "REAL name pakeistas i:" << client->get_real_name() << "\n";
	client->welcome();
}

void Commands::nick_command(Client *client, std::string cmd, std::string line)
{
	std::vector<std::string> args;

	if (client->is_new())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	std::string nick = line.substr(0, line.find_first_of(WHITESPACES));

	if (nick.empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NONICKNAMEGIVEN, cmd));

	if (nick[0] == '@')
		return client->reply(responce_msg(client->get_nick_name(), ERR_ERRONEUSNICKNAME, nick));

	if (client->get_nick_name() != nick)
	{
		if (_server->get_client(nick))
			return client->reply(responce_msg(client->get_nick_name(), ERR_NICKNAMEINUSE, nick));
		client->set_nick_name(nick);
		std::cout << "NICK name pakeistas i:" << client->get_nick_name() << "\n";
		client->welcome();
	}
}

void Commands::join_command(Client *creator, std::string cmd, std::string args)	{
	(void) cmd;
	if (creator->get_status() < REGISTERED)
		return creator->reply(responce_msg(creator->get_nick_name(), ERR_NOTREGISTERED, ""));
	std::vector<std::string>	names;
	if (args.empty())
		return creator->reply(" 462 : Need more parameters.\r\n");
	size_t i = args.find(',', 0);
	if (i == std::string::npos)
		names.push_back(args);
	else	{
		while (i != std::string::npos)	{
			names.push_back(args.substr(0, i));
			args = args.substr(i + 1);
			if (args[0] != '#')
				break ;
			i = args.find(',', 0);
			if (i == std::string::npos)
				names.push_back(args);
		}
	}
	for (unsigned int i = 0; i < names.size(); i++)	{
		if (!i)	{
			if (names[i][i] != '#')
				names[i] = "#" + names[i];
		}
	//	std::cout << " NAME[" << i << "] " << names[i] << std::endl;
	Channel	*exists;
//	args = args.substr(0, args.find_first_of(WHITESPACES));
	exists = _server->get_channel(names[i]);
	if (exists)
		(*exists).newUser(creator);
	else	{
		_server->get_channels().push_back(new Channel(_server, creator, names[i]));
		/*return*/ _server->get_channels().back()->newUser(creator);
	}
	}
}

void Commands::pmsg_command(Client *client, std::string cmd, std::string line)
{
	size_t i;
	std::string	msg = "";
	std::string send_msg;

	if (!client->is_registered() && !client->is_admin())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	std::string nick = line.substr(0, line.find_first_of(WHITESPACES));

	if (nick.empty())
		return client->reply(" 411 : No recipient given\r\n");
	i = line.find_first_not_of(WHITESPACES, nick.size());
	if (i != std::string::npos)
		msg = line.substr(i, line.size());
	if (msg.empty())
		return client->reply(" 412 : No text to send\r\n");
	if (nick[0] != '#')	{
		Client *receiver = _server->get_client(nick);
		if (!receiver)
			return client->reply(" 401 :No such nick\r\n");
		send_msg = " " + cmd + " " + nick + " ";
		if (msg[0] != ':')
			send_msg = client->sendMsg(send_msg + ":" + msg);
		else
			send_msg = client->sendMsg(send_msg + msg);
		std::cout << send_msg << std::endl;
		send(receiver->get_fd(), send_msg.c_str(), send_msg.length(), 0);
	}
	else	{
		Channel	*channel;
		channel = _server->get_channel(nick);
		if (!channel)
			return client->reply(" 403 :No such channel\r\n");
		if (!getClientByNick(channel->getUsers(), client->get_nick_name()))
			return client->reply(" 404 :Cannot send to channel, client isn't in the channel\r\n");

		std::cout << line << std::endl;
//		std::cout << channel->getName() << std::endl;
		msg = ":" +  client->fullID() + " " + cmd + " " + line + "\r\n";
		channel->broadcast(client, msg);
	}
}

void Commands::oper_command(Client *client, std::string cmd, std::string line)
{
	size_t i;

	if (client->is_admin())
		return;

	if (!client->is_registered())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	std::string name = line.substr(0, line.find_first_of(WHITESPACES));
	if (name.empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	i = line.find_first_not_of(WHITESPACES , name.size());
	if (i == std::string::npos)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	std::string pass = line.substr(i, line.find_first_of(WHITESPACES, i) - i);

	if (_server->get_admin_name() != name || _server->get_admin_pass() != pass)
		return client->reply(responce_msg(client->get_nick_name(), ERR_PASSWDMISMATCH, ""));

	client->set_status(client->get_status() + 1);
	return client->reply(responce_msg(client->get_nick_name(), RPL_YOUREOPER, ""));
}

void Commands::ison_command(Client *client, std::string cmd, std::string line)
{
	std::string msg = "";
	std::vector<std::string> args;

	if (!client->is_registered())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	get_arguments(line, &args);

	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it)
		if (_server->get_client(*it))
			msg += *it + " ";
		
	client->reply(responce_msg(msg, RPL_ISON, cmd));
}


void Commands::pong_command(Client *client, std::string cmd, std::string line)
{
	(void) cmd;

	if (!client->is_registered() && !client->is_admin())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	return client->reply(" PONG :" + line + "\r\n");
}

void Commands::part_command(Client *client, std::string cmd, std::string args)	{
	(void) cmd;
	if (client->get_status() < REGISTERED)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOTREGISTERED, ""));
	if (args.empty())
		return client->reply(" 462 : Need more parameters.\r\n");
	Channel	*exists;
	args = args.substr(0, args.find_first_of(WHITESPACES));
	exists = _server->get_channel(args);
	if (!exists)
		return client->reply(" 403 : No such channel.\r\n");
	if (!(*exists).isOnChan(client))
		return client->reply(" 442 : Not on channel.\r\n");
	 client->part(exists);
}

void Commands::tpic_command(Client *client, std::string cmd, std::string args)
{
	(void)cmd;
	size_t i;
	std::string	msg;
	if (client->get_status() < REGISTERED)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOTREGISTERED, ""));
	if (args.empty())
		return client->reply(" 462 : Need more parameters.\r\n");
	std::string name = args.substr(0, args.find_first_of(WHITESPACES));
	Channel	*channel;
	channel = _server->get_channel(name);
	if (!channel)
		return client->reply(" 403 :No such channel\r\n");
	if (!getClientByNick(channel->getUsers(), client->get_nick_name()))
		return client->reply(" 404 :Cannot send to channel, client isn't in the channel\r\n");
	i = args.find_first_not_of(WHITESPACES, name.size());
	if (i != std::string::npos)
		msg = args.substr(i, args.size());
	if (msg.empty())
		channel->topic(client);
	else
		channel->topic(client, msg);
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
	i = 0;
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
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOTREGISTERED, ""));
	if (_server->get_channels().empty())
		return client->reply(" 323 " + client->get_nick_name() + " :No existant channels.\r\n");
	std::vector<Channel *>::iterator it = _server->get_channels().begin();
	for (; it != _server->get_channels().end(); it++)	{
		client->reply(" 322 " + client->get_nick_name() + " " + (*it)->getName() + " " + uitos((*it)->getUsers().size()) + " :" + (*it)->getTopic() + "\r\n");
	}
	return client->reply(" 323 " + client->get_nick_name() + ": No more channels.\r\n");
}

void Commands::mode_command(Client *client, std::string cmd, std::string line)
{
	std::vector<std::string> args;

	if (!client->is_registered())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	get_arguments(line, &args);

	if (args.size() > 0 && args[0][0] =='#')
	{
		if (args.size() == 1)
			return client->reply(responce_msg(client->get_nick_name(), RPL_UMODEIS, ""));

		if (args.size() < 3)
			return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

		Channel *channel = _server->get_channel(args[0]);
		if (!channel)
			return client->reply(responce_msg(client->get_nick_name(), ERR_NOSUCHCHANNEL, args[0]));

		Client *target = _server->get_client(args[2]);
		if (!target)
			return client->reply(responce_msg(client->get_nick_name(), ERR_NOSUCHNICK, args[2]));

		if (!channel->isOnChan(client) || !channel->isOnChan(target))
			return client->reply(responce_msg(client->get_nick_name(), ERR_NOTONCHANNEL, args[0]));

		if (!channel->isChanOp(client))
			return client->reply(responce_msg(client->get_nick_name(), ERR_CHANOPRIVSNEEDED, args[0]));

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
						(*it)->reply(" MODE " + args[0] + " " + plusminus + mode[i] + " " + target->get_nick_name() + "\r\n");
					break ;
				}
				default:
					return client->reply(responce_msg(client->get_nick_name(), ERR_UMODEUNKNOWNFLAG, ""));
			}
		}
	}
}

void Commands::who_command(Client *client, std::string cmd, std::string line)
{
	(void) cmd;

	if (!client->is_registered())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	std::string name = line.substr(0, line.find_first_of(WHITESPACES));
	if (name.empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	if (name[0] == '#')
	{
		Channel *channel = _server->get_channel(name);
		if (!channel)
			return client->reply(responce_msg(client->get_nick_name(), ERR_NOSUCHCHANNEL, name));

		for (std::vector<Client *>::iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it)
			client->reply(responce_msg(client->get_nick_name(), RPL_WHOSPCRPL, " " + name + " " + (*it)->get_user_name() + " " + (*it)->get_hostname() + " " + _server->get_name() + " " + (*it)->get_nick_name() + " H 0 " + (*it)->get_real_name()));
		return client->reply(responce_msg(client->get_nick_name(), RPL_ENDOFWHO, name));
	}

	Client *target = _server->get_client(name);
	if (!target)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOSUCHNICK, name));

	if (target->get_channels().empty())
		client->reply(responce_msg(client->get_nick_name(), RPL_WHOSPCRPL, " * " + target->get_user_name() + " " + target->get_hostname() + " " + _server->get_name() + " " + target->get_nick_name() + " H 0 " + target->get_real_name()));
	else
		client->reply(responce_msg(client->get_nick_name(), RPL_WHOSPCRPL, " " + target->get_channels()[0]->getName() + " " + target->get_user_name() + " " + target->get_hostname() + " " + _server->get_name() + " " + target->get_nick_name() + " H 0 " + target->get_real_name()));
	return client->reply(responce_msg(client->get_nick_name(), RPL_ENDOFWHO, name));
}

void Commands::kick_command(Client *client, std::string cmd, std::string line)
{
	(void) cmd;
	(void) client;
	(void) line;

	std::vector<std::string> args;

	if (!client->is_registered())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	get_arguments(line, &args);

	if (args.size() < 2)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	if (args[0][0] != '#')
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOCHANELNAME, ""));

	Channel *channel = _server->get_channel(args[0]);
	if (!channel)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOSUCHCHANNEL, args[0]));

	if (!channel->isChanOp(client))
		return client->reply(responce_msg(client->get_nick_name(), ERR_CHANOPRIVSNEEDED, args[0]));

	Client *target = _server->get_client(args[1]);
	if (!target)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOSUCHNICK, args[1]));

	for (std::vector<Client *>::iterator it = channel->getChops().begin(); it != channel->getChops().end(); ++it)
		if ((*it)->get_nick_name() == args[1])
		{
			channel->getChops().erase(it);
			break;
		}
	for (std::vector<Client *>::iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it)
		if ((*it)->get_nick_name() == args[1])
		{
			channel->getUsers().erase(it);
			break;
		}

	for (std::vector<Client *>::iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it)
	{
		if (args.size() == 3 && !args[2].empty())
			(*it)->reply(" KICK " + args[0] + " " + args[1] + " " + args[2] + "\r\n");
		else
			(*it)->reply(" KICK " + args[0] + " " + args[1] + " Your behavior is not conducive to the desired environment.\r\n");
	}
}

void Commands::quit_command(Client *client, std::string cmd, std::string args)	{
//	(void) cmd;
//	if (client->get_status() < REGISTERED)
//		return client->reply(responce_msg(client->get_nick_name(), ERR_NOTREGISTERED, ""));
	(void)args;
//	if (args.empty())
//		return client->reply(" 462 : Need more parameters.\r\n");
	client->reply( " " + cmd + " " + client->get_nick_name() +  " :Bye Irc");
//	std::cout << client->get_fd() << " quitting fd\n";
	_server->client_quit(client->get_fd());
//	client->part(exists);

}

void Commands::kill_command(Client *client, std::string cmd, std::string line)
{
	if (!client->is_admin())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOPRIVILEGES, ""));
	
	std::string nick = line.substr(0, line.find_first_of(WHITESPACES));

	if (nick.empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	Client *target = _server->get_client(nick);
	if (!target)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOSUCHNICK, nick));
	target->reply(responce_msg("", RPL_KILLDONE, nick + " :Your behavior is not conducive to the desired environment"));

	quit_command(target, "QUIT", "");
}

void Commands::squi_command(Client *client, std::string cmd, std::string args)	{
	if (!client->is_admin())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOPRIVILEGES, ""));
	(void) cmd;
	(void)args;
	_server->broadcast_to_all_clients("Server is going down now.");
	Server::turn_off(1);
}

void Commands::wall_command(Client *client, std::string cmd, std::string args)
{
	if (!client->is_admin())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOPRIVILEGES, ""));

	if (args.empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, cmd));

	_server->broadcast_to_all_clients(args);
}

