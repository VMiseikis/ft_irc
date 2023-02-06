#include "LoveBot.hpp"

LoveBot::LoveBot(std::string ip, std::string port, std::string pass, std::string nick): _ip(ip), _pass(pass), _nick(nick), _join(false)	{
//	std::cout << _nick << std::endl;
	int temp = std::stoi(port); //atoi??
	if (temp > 63535 || temp < 1)
		throw (std::range_error("Bad port value"));
	_port = temp;
	getSocket();
}

void	LoveBot::getSocket(void)	{
	_fd = socket(PF_INET, SOCK_STREAM, 0);//AF_INET
	if (_fd < 0)
		throw (std::range_error("Failed geting a socket"));
	//fill in hint struct
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(_port);
	inet_pton(AF_INET, _ip.c_str(), &hint.sin_addr);

	//connect to server
	int conn = connect(_fd, (SA *)&hint, sizeof(hint));
	if (conn < 0)	{
		std::cerr<< " connect fail" << std::endl;
		throw (std::range_error( "coonect"));
	}
	const int	yes = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		throw (std::range_error("Failed setting socket options"));
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
		throw (std::range_error("Failed to set nonblock"));
}

LoveBot::~LoveBot(void){
	close(_fd);
};

void	LoveBot::sendMsg(const std::string &msg)	{
	std::string m = msg + "\r\n";
	int sent = send(_fd, m.c_str(), m.length(), 0);
	if (sent < 0)	{
		throw (std::runtime_error("Send() failure"));
	}
	std::cout << "Sent(" << sent << "): " + m << std::endl;
}

void	LoveBot::signIn(void)	{
	sendMsg("PASS " + _pass);
	sendMsg("NICK " + _nick);
	sendMsg("USER superUser 0 * :loveBot v1.0");
}

void	LoveBot::run(void)	{
	getJokes();
	signIn();
	struct pollfd	fd[1];
	memset(fd, 0, sizeof(fd));
	fd[0].fd = _fd;
	fd[0].events = POLLIN;
	int ret;

	while	(_on)	{
		ret = poll(fd, 1, -1);
		if (ret == -1)	{
			if (_on)
				throw (std::runtime_error("poll == -1"));
			else
				throw (std::runtime_error("\b\bBot finished"));
		}
		if ((fd[0].revents & POLLIN) == POLLIN)	{
			recieveMsg();
		}
		if ((fd[0].revents & POLLHUP) == POLLHUP)
			throw (std::runtime_error("Server dc'ed"));
	}
}

void	LoveBot::recieveMsg(void)	{
	char		buff[512];
	int			bytes;
	std::string	msg;

	memset(buff, 0, 512);
	while (!strstr(buff, "\r\n"))	{
		memset(buff, 0, 512);
		bytes = recv(_fd, buff, 511, 0);
		if (bytes == -1 && _on)
			throw (std::runtime_error("Recv failure"));
		if (bytes == 0)
			throw (std::runtime_error("Server closed connection"));
		msg += buff;
	}
	std::cout <<"Received: " << msg << std::endl;
	readMsg(msg);
}

void	LoveBot::readMsg(std::string msg)	{
	std::vector<std::string>	args;

	if (msg.empty())
		return ;
	size_t i = msg.find(' ', 0);
	while (i != std::string::npos)	{
		args.push_back(msg.substr(0, i));
		msg = msg.substr(i + 1);
		i = msg.find(' ', 0);
		if (msg[0] == ':')	{
			args.push_back(msg.substr(0, msg.find_last_not_of(" \r\n\t\v") + 1));
			break ;
		}
		if (i == std::string::npos)
			args.push_back(msg);
	}

	i = args[0].find('!', 0);
	if (i != std::string::npos)
		args[0] = args[0].substr(1, i - 1);
	for (int i = 0; i != args.size(); i++)	{
		std::cout << args[i] << "| args\n"; 
	}
	respond(args);
}

void	LoveBot::respond(std::vector<std::string> &args)	{
	if (isdigit(args[1][0]))	{
		if (args[1] == "001")
			return sendMsg("OPER admin pass");
		if (!_join)	{
			_join = true;
			return sendMsg("JOIN #Jokes");
		}
//		if /*(args[1] == "381")*/
//			return sendMsg("JOIN #Jokes");
		return ;
	}
	if (args[1] == "JOIN")	{
		if (args[0] == _nick)
			return sendMsg("TOPIC #Jokes :!info & HAHA!!! <3");
		return sendMsg("PRIVMSG " + args[2].substr(1) + " :HAI! " + args[0] + " <3"); 
	}
	if (args[1] == "NOTICE" || args[1] == "PRIVMSG")	{
		if (args[2][0] == '#')	{
			if (args[3] == ":!joke")
				return tellJoke(args);
			if (args[3] == ":!info")
				return sendMsg(args[1] + " " + args[2] + " :!joke for a joke! <3");
		}
		else
			return sendMsg(args[1] + " " + args[0] + " :Hallonchen! <3");
	}
}

void	LoveBot::getJokes(void)	{
	std::ifstream	ifs("./jokes.txt");
	if (!ifs.is_open())	{
		_joke.push_back("Somebody f****d up my jokes, sorry.. <3");
	}
	else	{
		std::string	joke;
		while (std::getline(ifs, joke))	{
			if (!joke.empty())
				_joke.push_back(joke);
		}
		ifs.close();
	}
}

void	LoveBot::tellJoke(std::vector<std::string> &args)	{
	if (_joke.size() == 1)
		return sendMsg(args[1] + " " + args[2] + " :" + _joke[0]);
	else	{
		int i = rand() % _joke.size();
		if (i % 3 == 1)
			sendMsg(args[1] + " " + args[2] + " :Here comes another! <3");
		if (i % 3 == 2)
			sendMsg(args[1] + " " + args[2] + " :Hope you get this one! <3");
		return sendMsg(args[1] + " " + args[2] + " :" + _joke[i]);
	}
}

bool	LoveBot::_on = true;

bool	LoveBot::isOn(void)	{
	return LoveBot::_on;
}

void	LoveBot::turnOff(int sig)	{
	(void)sig;
	LoveBot::_on = false;
}
