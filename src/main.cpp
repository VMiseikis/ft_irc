/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmiseiki <vmiseiki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 16:51:15 by vmiseiki          #+#    #+#             */
/*   Updated: 2023/02/09 16:51:19 by vmiseiki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "signal.h"

static void nichtGut(std::string msg)	{
	std::cout << msg << std::endl;
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		nichtGut("Usage: <ircserv> <port> <password>");
		return (0);
	}

	for (int i = 0; argv[1][i] != '\0'; i++) {
		if (!isdigit(argv[1][i])) {
			nichtGut("Sorry, the port doesn't feel right");
			return (0);
		}
	}

	int port = atoi(argv[1]);
	if (port < 1000 || port > 65535) {
		nichtGut("It feels like the port is a bit of range");
		return (0);
	}

	for (int i = 0; argv[2][i] != '\0'; i++) {
		if (!isprint(argv[2][i])) {
			nichtGut("Unacceptable password format");
			return (0);
		}
	}

	Server srv(port, argv[2]);
	try {
		signal(SIGINT, Server::turn_off);
		srv.run_server();
		return (0);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
