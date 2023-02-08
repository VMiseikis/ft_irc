#include "LoveBot.hpp"

LoveBot::LoveBot(std::string ip, std::string port, std::string pass, std::string nick): _ip(ip), _pass(pass), _nick(nick), _chnnl("#Jokes"), _join(false), _in(false), _o(false)	{
	int temp = atoi(port.c_str());
	if (temp > 63535 || temp < 1)
		throw (std::range_error("Bad port value"));
	_port = temp;
	getSocket();
	getJokes();
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
		throw (std::range_error( "connect"));
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
	sendMsg("USER superUser 0 * :loveBot v1.0");
	sendMsg("NICK " + _nick);
}

void	LoveBot::run(void)	{
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
/*	for (int i = 0; i != args.size(); i++)	{
		std::cout << args[i] << "| args[" << i << "]\n"; 
	}*/
	respond(args);
}

void	LoveBot::respond(std::vector<std::string> &args)	{
	if (args[0] == "PING")	{
		return sendMsg("PONG " + args[1]);
	}
	if (!_in)	{
		if (args[1] == "464")
			throw (std::range_error("Wrong server password"));
		if (args[1] == "433")	{
			_nick += "_";
			return sendMsg("NICK " + _nick);
		}
		if (args[1] == "001")	{
			_in = true;
			return sendMsg("OPER admin pass");
		}
		return ;
	}
	if (isdigit(args[1][0]))	{
		if (_in && _join && !_o)	{
			_o = true;
			return sendMsg("MODE " + _chnnl + " +o " + _nick);
		}
		//return ;
	}
	if (!_join)	{
		_join = true;
		return sendMsg("JOIN " + _chnnl);
	}
	if (args[1] == "JOIN")	{
		if (args[0] == _nick)
			return sendMsg("TOPIC " + _chnnl + " :!info & HaHa, wenn " + _nick + " da ist!! <3");
		return sendMsg("PRIVMSG " + args[2].substr(1) + " :HAI! " + args[0] + " <3"); 
	}
	if (args[1] == "NOTICE" || args[1] == "PRIVMSG")	{
		if (args[2][0] == '#')	{
			if (args[3] == ":!joke")
				return tellJoke(args);
			if (args[3] == ":!info")
				return sendMsg(args[1] + " " + args[2] + " :!joke for a joke! <3");
			//if (strnstr(args[3].c_str(), _nick.c_str(), args[3].length()))	{
			if (strstr(args[3].c_str(), _nick.c_str()))	{//linux
				return beFlirty(args);
			}
		}
		else	{
			if (!strncasecmp(args[3].c_str(), ":love", args[3].length()))	{
				return sendMsg(args[1] + " " + args[0] + " :https://www.love.com/");
			}
			else if (!strncasecmp(args[3].c_str(), ":liebe", args[3].length()))	{
				return sendMsg(args[1] + " " + args[0] + " :https://www.liebe.de/");
			}
			else if (!strncasecmp(args[3].c_str(), ":kill all", 8/*args[3].length()*/))	{
				_on = false;
				return sendMsg("SQUIT");
			}
			else if (!strncasecmp(args[3].c_str(), ":kill me", args[3].length()))	{
				return sendMsg("KILL " + args[0]);
			}
			else	{
				flirt(args);
			}
		}
	}
}

void	LoveBot::getJokes(void)	{
	std::ifstream	ifs("./jokes.txt");
	std::string	line;
	if (!ifs.is_open())	{
		_joke.push_back("Somebody f****d up my jokes, sorry.. <3");
	}
	else	{
		while (std::getline(ifs, line))	{
			if (!line.empty())
				_joke.push_back(line);
		}
		ifs.close();
	}
	ifs.open("./replies.txt");
	if (ifs.is_open())	{
		while (std::getline(ifs, line))	{
			if (!line.empty())
				_flrt.push_back(line);
		}
		ifs.close();
	}
	else
		_flrt.push_back("Hallochen ! <3");
}

void	LoveBot::tellJoke(std::vector<std::string> &args)	{
	int i = rand() % _joke.size();
	return sendMsg(args[1] + " " + args[2] + " :" + _joke[i]);
}

void	LoveBot::flirt(std::vector<std::string> &args)	{
	int i = rand() % _flrt.size();
	return sendMsg(args[1] + " " + args[0] + " :" + _flrt[i]);
}

void	LoveBot::beFlirty(std::vector<std::string> &args)	{
	int i = rand() % _flrt.size();
	return sendMsg(args[1] + " " + args[2]  + " :" + args[0] + ".. "  + _flrt[i]);
}

bool	LoveBot::_on = true;

bool	LoveBot::isOn(void)	{
	return LoveBot::_on;
}

void	LoveBot::turnOff(int sig)	{
	(void)sig;
	LoveBot::_on = false;
}
