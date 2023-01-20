#include "server.hpp"
#include "commands.hpp"

Commands::Commands()
{
	_commands.insert(std::make_pair("PASS", &Commands::pass_command));
	_commands.insert(std::make_pair("NICK", &Commands::nick_command));
	_commands.insert(std::make_pair("USER", &Commands::user_command));

}






bool Commands::execute_command(Client *client, std::string cmd, std::vector<std::string> args)
{
	try {
		(this->*_commands.at(cmd))(client, args);
		
	}
	catch (const std::out_of_range &err) { return false; }
	return true;
}

void Commands::pass_command(Client *client, std::vector<std::string>args)
{
	(void) args;
	if (client->get_status() != NEW)
		return ;

	std::cout << "password is matching\n";

	client->set_status(HANDSHAKE);	
}

void Commands::nick_command(Client *client, std::vector<std::string>args)
{
	(void) args;
	if (client->get_status() == NEW)
		return ;
	std::cout << "nick was changed\n";

	client->set_nick_name("TEST_NICK");
}

void Commands::user_command(Client *client, std::vector<std::string> args)
{
	(void) args;
	if (client->get_status() == NEW)
		return ;
	std::cout << "user was changed\n";

	client->set_user_name("USER_NICK");
}