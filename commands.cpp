#include "commands.hpp"

Commands::Commands(Server *server) : _server(server)
{
	_commands.insert(std::make_pair("PASS", &Commands::pass_command));
	_commands.insert(std::make_pair("NICK", &Commands::nick_command));
	_commands.insert(std::make_pair("USER", &Commands::user_command));

}

bool Commands::execute_command(Client *client, std::string cmd, std::vector<std::string> args)
{

	try {
		(this->*_commands.at(cmd))(client, args);
	} catch (const std::out_of_range &err) { return false; }
	return true;
}

std::string responce_msg(std::string client, int err, std::string cmd)
{
	switch (err)
	{
		case ERR_NEEDMOREPARAMS:
			return (" 461 " + client + " " + cmd + " :Not enough parameters\r\n");
		case ERR_ALREADYREGISTRED:
			return (" 462 " + client + " :Unauthorized command\r\n");
		case ERR_PASSWDMISMATCH:
			return (" 464 " + client + " :Password incorrect\r\n");
	}
	return "";
}

void Commands::pass_command(Client *client, std::vector<std::string>args)
{
	if (args.size() < 2)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, args[0]));

	if (!client->is_new())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	if (_server->get_password() != args[1])
		return client->reply(responce_msg(client->get_nick_name(), ERR_PASSWDMISMATCH, ""));

	client->set_status(client->get_status() + 1);	
}

void Commands::nick_command(Client *client, std::vector<std::string>args)
{
	(void) args;

	if (client->get_status() < HANDSHAKE)
		return ;


	std::cout << "nick was changed\n";

	client->set_nick_name("TEST_NICK");
}

void Commands::user_command(Client *client, std::vector<std::string> args)
{

	if (!client->is_auth())
		return client->reply(responce_msg(client->get_nick_name(), ERR_ALREADYREGISTRED, ""));

	if (args.size() < 5)
		return client->reply(responce_msg(client->get_nick_name(), ERR_NEEDMOREPARAMS, args[0]));

	client->set_user_name(args[1]);
	client->set_real_name((args[4].substr(args[4].find_first_not_of(' ' , 1), args[4].size())));
	client->set_status(client->get_status() + 1);
}
