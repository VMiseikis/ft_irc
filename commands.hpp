#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include <map>

class Commands
{
	public:
		Commands();
		~Commands();
		bool execute_command(std::string cmd, std::vector<std::string> args);

	private:
		std::map<std::string, void (Commands::*)(std::vector<std::string> args)> _commands; //Thank you CPP Module 01 ex05

		void nick_command(std::vector<std::string> args);
		void user_command(std::vector<std::string> args);



};

#endif
