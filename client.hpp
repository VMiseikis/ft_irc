#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include "ft_irc.hpp"

enum client_state { NEW, HANDSHAKE, REGISTERED, OPERATOR};


class Client
{
	private:
		int			_fd;
		int 		_port;
		int			_status;
		std::string _ip;
		std::string _user;
		std::string	_real;
		std::string _nick;
		std::string _pass;
//		std::string	_mode;

	public:
		Client(int fd, std::string ip, int port);
		~Client();

		int get_fd();
		int get_port();
		int get_status();
		std::string get_ip();
		std::string get_user_name();
		std::string get_real_name();
		std::string get_nick_name();
		std::string get_password();

		void welcome();

		void set_status(int status);
		void set_user_name(std::string user);
		void set_real_name(std::string real);
		void set_nick_name(std::string nick);
		void set_password(std::string pass);

		bool is_new();
		bool is_auth();
		bool is_registered();
		bool is_operator();

		void reply(const std::string &msg);
		std::string	sendMsg(std::string msg);
		std::string	fullID(void);
	
};

Client	*getClientByNick(std::vector<Client *> &from, std::string nick);

#endif
