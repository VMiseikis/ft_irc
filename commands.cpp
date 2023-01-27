#include "commands.hpp"

Commands::Commands(Server *server) : _server(server)
{
	_commands.insert(std::make_pair("PASS", &Commands::pass_command));
	_commands.insert(std::make_pair("NICK", &Commands::nick_command));
	_commands.insert(std::make_pair("USER", &Commands::user_command));
	_commands.insert(std::make_pair("PING", &Commands::ping_command));
//	_commands.insert(std::make_pair("PONG", &Commands::pong_command));
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
		case ERR_NONICKNAMEGIVEN:
			return (" 431 " + client + " " + arg + " :No nickname give to change to.\r\n");
		case ERR_ERRONEUSNICKNAME:
			return (" 432 " + client + " " + arg + " :Erroneus nickname.\r\n");
		case ERR_NICKNAMEINUSE:
			return (" 433 " + client + " " + arg + " :Nickname is already in use.\r\n");

		case ERR_NOTREGISTERED:
			return (" 451 " + client + " :You have not registered.\r\n");

		case ERR_NEEDMOREPARAMS:
			return (" 461 " + client + " " + arg + " :Not enough parameters\r\n");
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

bool Commands::execute_command(Client *client, std::string cmd, std::vector<std::string> args)
{
	if (cmd != "PASS" && client->is_new())
	{
		client->reply(responce_msg(client->get_nick_name(), ERR_NOTREGISTERED, ""));
		return false;
	}

	try {
		(this->*_commands.at(cmd))(client, args);
	} catch (const std::out_of_range &err) { return false; }
	return true;
}

void Commands::pass_command(Client *client, std::vector<std::string>args)
{
	if (args.size() < 2)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, args[0]));

	if (!client->is_new())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	if (_server->get_password() != args[1])
		return client->reply(responce_msg(client->get_nick_name(), ERR_PASSWDMISMATCH, ""));
	std::cout << "PASS sutampa su:" << args[1] << "\n";
	client->set_status(client->get_status() + 1);	
}

void Commands::nick_command(Client *client, std::vector<std::string>args)
{

	if (client->is_new())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));
	
	if (args.size() < 2)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NONICKNAMEGIVEN, args[0]));

	if (!isalpha(args[1][0]))
		return client->reply(responce_msg(client->get_nick_name(), ERR_ERRONEUSNICKNAME, args[1]));
	for (unsigned long i = 1; args[1][i]; i++)
		if (!iswalnum(args[1][i]))
			return client->reply(responce_msg(client->get_nick_name(), ERR_ERRONEUSNICKNAME, args[1]));
	
	if (client->get_nick_name() != args[1])
	{
		if (_server->get_client(args[1]))
			return client->reply(responce_msg(client->get_nick_name(), ERR_NICKNAMEINUSE, args[1]));
		client->set_nick_name(args[1]);
		std::cout << "NICK name pakeistas i:" << client->get_nick_name() << "\n";
		client->welcome();
	}

	// if (!client->is_auth() && !client->get_user_name().empty() && !client->get_real_name().empty())
	// {
	// 	client->set_status(client->get_status() + 1);
	// 	client->reply(client->welcome_message());
	// }
}

void Commands::user_command(Client *client, std::vector<std::string> args)
{
	if (!client->is_auth() || !client->get_user_name().empty())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	if (args.size() != 5)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, args[0]));

	client->set_user_name(args[1]);
	std::cout << "USER name pakeistas i:" << client->get_user_name() << "\n";

	client->set_real_name((args[4].substr(args[4].find_first_not_of(' '), args[4].size())));
	std::cout << "REAL name pakeistas i:" << client->get_real_name() << "\n";
	client->welcome();

	//client->set_status(client->get_status() + 1);
}

void Commands::ping_command(Client *client, std::vector<std::string> args)
{
	if (!client->is_registered())
		return client->reply(responce_msg(client->get_nick_name(), ERR_NOTREGISTERED, ""));

	if (args.size() < 2)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, args[0]));
	
	client->reply(responce_msg(client->get_user_name(), PINGRESPONCE, args[1]));
}


void Commands::join_command(Client *creator, std::vector<std::string> args)	{
	if (args.size() == 1)
		return creator->reply(" 462 : Need more parameters.\r\n");
	Channel	*exists;
	exists = _server->getChannel(args[1]);
	if (exists)
		(*exists).addUser(creator);
	else
		_server->getChannels().push_back(new Channel(creator, args[1]));
}

void	Commands::pmsg_command(Client *client, std::vector<std::string> args)	{
	std::string	msg;
	std::cout << "pmsg\n";
	if (args.size() == 1)	{
		return client->reply(" 411 : No recipient given\r\n");
	}
	if (args.size() == 2)	{
		return client->reply(" 412 : No no text to send\r\n");
	}
	if (args[1][0] == '#')	{
		Channel	*channel;
		channel = _server->getChannel(args[1]);
		if (!channel)	{
			return client->reply(" 403 :No such channel\r\n");
			//err_nochan
//			return ;
		}
		std::cout << channel->getName() << std::endl;
		return channel->broadcast(client, args);
	}
		
Client	*receiver;
		receiver = _server->get_client(args[1]);
		if (receiver)	{
//			if (args[2].empty())
//				return ;
			msg = " " + args[0] + " " + args[1]; 
			msg = client->sendMsg(msg + " :" + args[2]);
//		std::cout << msg << std::endl;
			send(receiver->get_fd(), msg.c_str(), msg.length(), 0);
			return ;
		}
		else	
			return client->reply(" 401 :No such nick\r\n");
}	



