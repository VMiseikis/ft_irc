#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "server.hpp"
#include "channel.hpp"

enum client_state { NEW, HANDSHAKE, REGISTERED, ADMIN};


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
		std::string _hostname;
		std::vector<Channel *>	_channels;

	public:
		Client(int fd, std::string ip, int port, std::string hostname);
		~Client();

		int get_fd();
		int get_port();
		int get_status();
		std::string get_ip();
		std::string get_user_name();
		std::string get_real_name();
		std::string get_nick_name();
		std::string get_password();
		std::string get_hostname();
		std::vector<Channel *> get_channels();

		void set_status(int status);
		void set_user_name(std::string user);
		void set_real_name(std::string real);
		void set_nick_name(std::string nick);
		void set_password(std::string pass);
		void set_hostname(std::string host);

		bool is_new();
		bool is_auth();
		bool is_registered();
		bool is_admin();

		void welcome();
		void reply(std::string id, std::string msg);
		//void reply(const std::string &msg);
		std::string	sendMsg(std::string msg);
		std::string	get_id();
		void	join(Channel *channel);
		void	part(Channel *channel);
		void	dc(void);
	
};

Client	*getClientByNick(std::vector<Client *> &from, std::string nick);

#endif
