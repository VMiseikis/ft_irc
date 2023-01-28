#include "commands.hpp"

Commands::Commands(Server *server) : _server(server)
{
	_commands.insert(std::make_pair("PASS", &Commands::pass_command));
	_commands.insert(std::make_pair("USER", &Commands::user_command));
	_commands.insert(std::make_pair("NICK", &Commands::nick_command));
	_commands.insert(std::make_pair("OPER", &Commands::oper_command));
	_commands.insert(std::make_pair("ISON", &Commands::ison_command));
	_commands.insert(std::make_pair("PING", &Commands::pong_command));


	_commands.insert(std::make_pair("JOIN", &Commands::join_command));
	_commands.insert(std::make_pair("PRIVMSG", &Commands::pmsg_command));

}

std::string responce_msg(std::string client, int err, std::string arg)
{
	switch (err)
	{
		// kolkas nznau kaip atskirti ar tai komanda ar tik kliento message
		// case ERR_UNKNOWNCOMMAND:
		// 	return (" 421 " arg + " :Unknown command.\r\n");

		//RPL_YOUREOPER (381)
		case RPL_ISON:
			return (" 303 :" + client + "\r\n");
		case RPL_YOUREOPER:
			return (" 381 " + client + " :You are now an IRC operator.\r\n");

		case ERR_NONICKNAMEGIVEN:
			return (" 431 " + client + " " + arg + " :No nickname give to change to.\r\n");
		case ERR_ERRONEUSNICKNAME:
			return (" 432 " + client + " " + arg + " :Erroneus nickname.\r\n");
		case ERR_NICKNAMEINUSE:
			return (" 433 " + client + " " + arg + " :Nickname is already in use.\r\n");

		case ERR_NOTREGISTERED:
			return (" 451 " + client + " :You have not registered.\r\n");

		case ERR_NEEDMOREPARAMS:
			return (" 461 " + client + " " + arg + " :Not enough, or to many parameters\r\n");
		case ERR_ALREADYREGISTRED:
			return (" 462 " + client + " :Unauthorized command\r\n");
		case ERR_PASSWDMISMATCH:
			return (" 464 " + client + " :Password incorrect\r\n");


		case PINGRESPONCE:
			return (" PONG :" + arg + "\r\n");


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
		(*args).push_back(line.substr(i, line.find_first_of(WHITESPACES, i) - i));
		i += (*args).back().length();
	}
}

void Commands::execute_command(Client *client, std::string line)
{
	size_t i;
	std::string args = "";
	std::string cmd = line.substr(0, line.find_first_of(WHITESPACES));
	
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::towupper);

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

	if (!iswalpha(nick[0]))
		return client->reply(responce_msg(client->get_nick_name(), ERR_ERRONEUSNICKNAME, nick));
	for (unsigned long i = 1; nick[i]; i++)
		if (!iswalnum(nick[i]))
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
	if (args.empty())
		return creator->reply(" 462 : Need more parameters.\r\n");
	Channel	*exists;
	args = args.substr(0, args.find_first_of(WHITESPACES));
	exists = _server->getChannel(args);
	if (exists)
		(*exists).newUser(creator);
	else	{
		_server->getChannels().push_back(new Channel(_server, creator, args));
	return _server->getChannels()[0]->newUser(creator);
	}
}

void Commands::pmsg_command(Client *client, std::string cmd, std::string line)
{
	size_t i;
	std::string	msg = "";
	std::string send_msg;

	if (!client->is_registered() && !client->is_operator())
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
		send(receiver->get_fd(), send_msg.c_str(), send_msg.length(), 0);
	}
	else	{
		Channel	*channel;
		channel = _server->getChannel(nick);
		if (!channel)	{
			return client->reply(" 403 :No such channel\r\n");
		}
//		std::cout << channel->getName() << std::endl;
		msg = ":" +  client->fullID() + " " + cmd + " " + line + "\r\n";
		channel->broadcast(client, msg);

		nick = channel->getTopic();
	if (!nick.empty())	{
		msg = ":" + client->fullID() /*_server->getName()*/ + " 332 " + client->get_nick_name();
		msg += " " + channel->getName() + " :" + nick + "\r\n";
		send(client->get_fd(), msg.c_str(), msg.length(), 0);
	}
		return ;
//		return channel->broadcast(client, args);
	}
}


void Commands::oper_command(Client *client, std::string cmd, std::string line)
{
	size_t i;

	if (client->is_operator())
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

	if (_server->get_oper_name() != name || _server->get_oper_pass() != pass)
		return client->reply(responce_msg(client->get_nick_name(), ERR_PASSWDMISMATCH, ""));

	client->set_status(client->get_status() + 1);
	return client->reply(responce_msg(client->get_nick_name(), RPL_YOUREOPER, ""));
}

void Commands::ison_command(Client *client, std::string cmd, std::string line)
{
	std::string msg = "";
	std::vector<std::string> args;

	if (!client->is_registered() && !client->is_operator())
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

	if (!client->is_registered() && !client->is_operator())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	std::string msg = "PONG :" + line;
	client->reply(msg);
	std::cout << "PONG responce:" << msg << std::endl;
}
