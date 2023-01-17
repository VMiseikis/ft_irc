#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include <map>

class Commands
{
	public:
		Commands();
		~Commands();
		void execute_command(std::string cmd);

	private:
		std::map<std::string, void (Commands::*)()> _commands; //Thank you CPP Module 01 ex05

		void nick_command();
		void name_command();



};

#endif
