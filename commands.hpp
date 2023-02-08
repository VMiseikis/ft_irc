#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#define USERLEN 10

#define RPL_UMODEIS 			221
#define RPL_ISON				303
#define RPL_ENDOFWHO			315
#define RPL_LISTSTART 			321
#define RPL_LIST 				322
#define RPL_LISTEND 			323
#define RPL_WHOREPLY 			352
#define RPL_NAMREPLY			353
#define RPL_WHOSPCRPL 			354
#define RPL_KILLDONE			361
#define RPL_ENDOFNAMES 			366
#define RPL_YOUREOPER			381
#define	ERR_NOSUCHNICK			401	
#define ERR_NOSUCHCHANNEL		403
#define	ERR_CANNOTSENDTOCHAN	404
#define	ERR_TOOMANYTARGETS		407
#define	ERR_NORECIPIENT			411
#define	ERR_NOTEXTTOSEND		412
#define ERR_UNKNOWNCOMMAND		421
#define ERR_NONICKNAMEGIVEN		431
#define ERR_ERRONEUSNICKNAME	432
#define ERR_NICKNAMEINUSE		433
#define ERR_NOTONCHANNEL 		442
#define ERR_NOTREGISTERED		451
#define ERR_NEEDMOREPARAMS		461
#define ERR_ALREADYREGISTRED	462
#define ERR_PASSWDMISMATCH		464
#define ERR_NOPRIVILEGES 		481
#define ERR_CHANOPRIVSNEEDED 	482
#define	ERR_UMODEUNKNOWNFLAG	501

#define	ERR_NOCHANELNAME		999

#define WHITESPACES				" \t\f\v\n\r"

#include <cstdlib>
#include "ft_irc.hpp"
#include "client.hpp"
#include "server.hpp"
#include "channel.hpp"

class Server;

class Commands
{
	public:
		Commands(Server *server);
		~Commands();

		void execute_command(Client *client, std::string line);

	private:
		Server *_server;
		std::map<std::string, void (Commands::*)(Client *client, std::string cmd, std::string args)> _commands;

		void get_arguments(std::string line, std::vector<std::string> *args);
		std::string responce_msg(int err, std::string client, std::string arg);

		void who_command(Client *client, std::string cmd, std::string args);
		void pass_command(Client *client, std::string cmd, std::string args);
		void user_command(Client *client, std::string cmd, std::string args);
		void nick_command(Client *client, std::string cmd, std::string args);
		void oper_command(Client *client, std::string cmd, std::string args);
		void ison_command(Client *client, std::string cmd, std::string args);
		void pong_command(Client *client, std::string cmd, std::string args);
		void mode_command(Client *client, std::string cmd, std::string args);
		void kick_command(Client *client, std::string cmd, std::string args);
		void kill_command(Client *client, std::string cmd, std::string args);
		void join_command(Client *client, std::string cmd, std::string args);
		void pmsg_command(Client *client, std::string cmd, std::string args);
		void part_command(Client *client, std::string cmd, std::string args);
		void tpic_command(Client *client, std::string cmd, std::string args);
		void list_command(Client *client, std::string cmd, std::string args);
		void quit_command(Client *client, std::string cmd, std::string args);
		void squi_command(Client *client, std::string cmd, std::string args);
		void wall_command(Client *client, std::string cmd, std::string args);
};

#endif
