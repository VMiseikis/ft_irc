#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "channel.hpp"

#define BACKLOG 0xFFFFFFF
#define BUFFER_LENGHT 100

class Channel;
class Commands;

class Server
{
	private:
		int 	_port;
		int 	_conn;
		int 	_server;
		static bool	_on;
		std::string	_name;
		std::string _password;
		std::string _admin_name;
		std::string _admin_pass;
		struct sockaddr_in _address;
	
		struct pollfd _pollfd;
		std::vector<struct pollfd> _pollfds;
		
		Commands *_cmd;
		std::map<int, Client *> _clients;
		std::vector<Channel *>	_channels;

		static bool	is_on(void);

		void new_server();
		void new_connection();
		void store_pollfd(int socket);
		void message_recieved(int fd);
		void handle_message(Client *client, std::string message);
		void client_disconnect(std::vector<struct pollfd>::iterator poll_it);
		
	public:
		Server(int port, std::string password);
		~Server();

		void run_server();
		void client_quit(int fd);
		static void	turn_off(int sig);
		void delete_channel(Channel *channel);
		void broadcast_to_all_clients(std::string msg);

		std::string	get_name();
		std::string get_password();
		std::string get_admin_name();
		std::string get_admin_pass();
		Client *get_client(std::string name);
		Channel	*get_channel(std::string &name);
		std::vector<Channel *> &get_channels();
		void	wall(const std::string &msg);
};

#endif
