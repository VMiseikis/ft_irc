#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#define USERLEN 10

#define ERR_UNKNOWNCOMMAND		421

#define ERR_NONICKNAMEGIVEN		431
#define ERR_ERRONEUSNICKNAME	432
#define ERR_NICKNAMEINUSE		433

#define ERR_NOTREGISTERED		451

#define ERR_NEEDMOREPARAMS		461
#define ERR_ALREADYREGISTRED	462
#define ERR_PASSWDMISMATCH		464

#define	ERR_NORECIPIENT			411
#define	ERR_CANNOTSENDTOCHAN	404
#define	ERR_NOSUCHNICK			401	
#define	ERR_NOTEXTTOSEND		412
#define	ERR_TOOMANYTARGETS		407


#define PINGRESPONCE			999

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
		void ping_command(Client *client, std::vector<std::string> args);

		void join_command(Client *client, std::vector<std::string> args);
		void list_command(Client *client, std::vector<std::string> args);
		void pmsg_command(Client *client, std::vector<std::string> args);



};

#endif
