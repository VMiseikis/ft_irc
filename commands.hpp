#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include <map>

#include "client.hpp"


class Commands
{
	public:
		Commands();
		~Commands();
		bool execute_command(Client *client, std::string cmd, std::vector<std::string> args);

	private:
		std::map<std::string, void (Commands::*)(Client *client, std::vector<std::string> args)> _commands; //Thank you CPP Module 01 ex05

		void pass_command(Client *client, std::vector<std::string> args);
		void nick_command(Client *client, std::vector<std::string> args);
		void user_command(Client *client, std::vector<std::string> args);



};

#endif
