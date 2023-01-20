#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#define USERLEN 10

#define ERR_NEEDMOREPARAMS		461
#define ERR_ALREADYREGISTRED	462
#define ERR_PASSWDMISMATCH		464

#include <map>
#include "client.hpp"
#include "server.hpp"

class Server;

class Commands
{
	public:
		Commands(Server *server);
		~Commands();
		bool execute_command(Client *client, std::string cmd, std::vector<std::string> args);

	private:
		Server *_server;

		std::map<std::string, void (Commands::*)(Client *client, std::vector<std::string> args)> _commands; //Thank you CPP Module 01 ex05


		void pass_command(Client *client, std::vector<std::string> args);
		void nick_command(Client *client, std::vector<std::string> args);
		void user_command(Client *client, std::vector<std::string> args);



};

#endif
