#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>

enum client_state { HANDSHAKE, REGISTERED, OPERATOR, ADMIN };


class Client
{
	private:
		int			_fd;
		int 		_port;
		int			_loged;
		std::string _ip;
		std::string _name;
		std::string _nick;
		std::string _pass;
		//client_state _state;

	public:
		Client(int fd, std::string ip, int port);
		~Client();

		int get_fd();
		int get_port();
		int get_login_status();
		std::string get_ip();
		std::string get_user_name();
		std::string get_nick_name();
		std::string get_password();

		void set_user_name(std::string name);
		void set_nick_name(std::string nick);
		void set_password(std::string pass);
		
		int is_registered();
};

#endif
