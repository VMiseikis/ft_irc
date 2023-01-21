#ifndef Channel_HPP
#define Channel_HPP

# include "ft_irc.hpp"
# include "client.hpp"


class	Client;

class	Channel	{
	private:
		std::string	_prefix; // & = local, #,  + = no modes or op except -t flag, ! = safe Channel  &#+ = std Channels
		std::string const	_name;	//case insensitive max-50(49)lenght
							//no ' ' ^G=ASCII 7 ',' 
							// ':' - Channel mask delimiter
		std::string				_topic;
		std::vector<Client *>	_chops;//TODO KICK MODE INVITE TOPIC
		std::vector<Client *>	_users;
		//std::vector<Client *>	_invite;

//		bool	nameOk(std::string	name);
	public:
		Channel(Client *creator, std::string & name);
		~Channel(void);
		std::string	const	getName(void) const; 
		void				addUser(Client	*client);
		bool				setTopic(std::string topic);
		
		std::vector<Client * > & getChops(void);
		std::vector<Client * > & getUsers(void);

//		broadcast(std::string const message);

};


#endif
