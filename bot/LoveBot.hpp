#ifndef LOVEBOT_HPP
#define LOVEBOT_HPP

# include "ft_irc.hpp"
# include <ctime>
#define SA struct sockaddr

class	LoveBot	{
	private:
		enum	responses {PRIVMSG, NOTICE, JOKE};
		int							_fd;
		std::string	const			_ip;
		unsigned short 				_port;
		std::string	const			_pass;
		std::string					_nick;
//		struct sockaddr_in			_addr;
		std::vector<std::string>	_joke;
		static bool					_on;
		static bool	isOn(void);
		void	sendMsg(const std::string &msg);
		void	getSocket(void);
		void	signIn(void);
		void	recieveMsg(void);
		void	readMsg(std::string msg);
		void	respond(std::vector<std::string> &args);
		void	getJokes(void);
		void	tellJoke(std::vector<std::string> &args);




	public:
		LoveBot(std::string ip, std::string port, std::string pass, std::string nick = "oOzka"); 
		~LoveBot(void);
		void		run(void);
		static void	turnOff(int sig);

};
#endif
