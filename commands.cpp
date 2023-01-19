#include "server.hpp"
#include "commands.hpp"

Commands::Commands()
{
	_commands.insert(std::make_pair("NICK", &Commands::nick_command));
	_commands.insert(std::make_pair("USER", &Commands::user_command));

}

bool Commands::execute_command(std::string cmd, std::vector<std::string> args)
{
	try {
		(this->*_commands.at(cmd))(args);
	}
	catch (const std::out_of_range &err) { return false; }
	return true;
}


void Commands::nick_command(std::vector<std::string>args)
{
	(void) args;
	std::cout << "nick was changed\n";
}

void Commands::user_command(std::vector<std::string> args)
{
	(void) args;
	std::cout << "name was changed\n";
}