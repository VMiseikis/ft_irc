# include "channel.hpp"

Channel::~Channel(void)	{}

Channel::Channel(Client *creator, std::string & name): _name(name)	{
//			if (nameOk(_name))	{
				_chops.push_back(creator);
				_users.push_back(creator);
//			}
}

std::string const	Channel::getName(void) const{
	return (_name);
}	

//		broadcast(std::string const message);

std::vector<Client * > & Channel::getChops(void)	{
	return (_chops);
}

std::vector<Client * > & Channel::getUsers(void)	{
	return (_users);
}

void	Channel::addUser(Client	*user)	{
	if (!getClientByNick(_users, (*user).get_nick_name()))
		//if user not already there 
		_users.push_back(user);
}
/*bool	Channel::nameOk(std::string	name)	{
	if (name.length() > 50 || name.find_first_of(" ,") != std::string::npos)
		return (false)
	for (int i = 0; i < name.length(); i++)	{
		if (!std::isprint(name[i]))
			return (false)
	}
	return (true);
}*/
