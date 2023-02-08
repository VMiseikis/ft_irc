#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "client.hpp"
#include "server.hpp"

class	Client;

class	Channel	{
	private:
		std::string 		_name;
		Server					*_server;
		std::string				_topic;
		std::vector<Client *>	_chops;//TODO KICK MODE INVITE TOPIC
		std::vector<Client *>	_users;
	public:
		Channel(Server *server, Client *creator, std::string & name);
		~Channel();
		std::string	const	getName(void) const; 
		void				addUser(Client	*client);
		
		std::vector<Client * > & getChops(void);
		std::vector<Client * > & getUsers(void);
		bool	isChanOp(Client *client);
		bool	isOnChan(Client *client);

		void	broadcast(Client *client, std::string msg);
		void	broadcast(std::string msg);
		void	newUser(Client *user);

		std::string	getTopic(void);
		std::string	getNamesList(void);
		void	names(Client *client);
		void	part(Client *client);
		void	dc(Client *client);
		void	topic(Client *client);
		void	topic(Client *client, std::string topic);

};

#endif
