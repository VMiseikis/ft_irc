#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>

class Client
{
	private:
		int			_fd;
		int 		_port;
		std::string _ip;
		std::string _name;

	public:
		Client(int fd, std::string ip, int port);
		~Client();

		int get_fd();
		int get_port();
		std::string get_ip();
		std::string get_user_name();

		void set_user_name(std::string name);
		
};

#endif
