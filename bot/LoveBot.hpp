#ifndef LOVEBOT_HPP
#define LOVEBOT_HPP

# include "ft_irc.hpp"
# include <ctime>
#define SA struct sockaddr

class	LoveBot	{
	private:
		int							_fd;
		std::string	const			_ip;
		unsigned short 				_port;
		std::string	const			_pass;
		std::string					_nick;
		std::string	const			_chnnl;
		std::vector<std::string>	_joke;
		std::vector<std::string>	_flrt;
		bool						_join;
		bool						_in;
		bool						_o;
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
		void	flirt(std::vector<std::string> &args);
		void	beFlirty(std::vector<std::string> &args);


	public:
		LoveBot(std::string ip, std::string port, std::string pass, std::string nick = "Aliona"); 
		~LoveBot(void);
		void		run(void);
		static void	turnOff(int sig);

};
#endif
