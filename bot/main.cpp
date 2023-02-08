#include "LoveBot.hpp"

int	main(int argc, char **argv)	{
	if (argc != 4)	{
		std::cout << "Usage: <exe*> <ip> <port> <pass>" << std::endl; 
		return (0);
	}
	try	{
		std::srand((unsigned int)time(NULL));
		LoveBot	bot(argv[1], argv[2], argv[3]);
		signal(SIGINT, LoveBot::turnOff);
		bot.run();
		return (0);
	}
	catch (std::exception &e)	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
