#include "server.hpp"
#include "commands.hpp"

Commands::Commands()
{
	_commands.insert(std::make_pair("NICK", &Commands::nick_command));
	_commands.insert(std::make_pair("NAME", &Commands::name_command));

}

void Commands::execute_command(std::string cmd)
{
	(this->*_commands.at(cmd))();
}


void Commands::nick_command()
{
	std::cout << "nick was changed\n";
}

void Commands::name_command()
{
	std::cout << "name was changed\n";
}