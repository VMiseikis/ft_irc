/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmiseiki <vmiseiki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 16:51:41 by vmiseiki          #+#    #+#             */
/*   Updated: 2023/02/09 17:12:06 by ajazbuti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LoveBot.hpp"

int	main(int argc, char **argv)	{
	if (argc != 4)	{
		std::cout << "\033[1;35mUsage: <exe*> <ip> <port> <pass>\033[1;0m" << std::endl; 
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
