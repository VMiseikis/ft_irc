#include "server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		exit(-1); 		//TODO handle wrong argument count

	for (int i = 0; argv[1][i] != '\0'; i++)
		if (!isdigit(argv[1][i]))
			exit(-1); 	//TODO handle incorect port format

	int port = atoi(argv[1]);
	if (port < 1000 || port > 65535)
		exit(-1); 		//TODO handle port out of range

	for (int i = 0; argv[2][i] != '\0'; i++)	
		if (!isprint(argv[2][i]))
			exit(-1); 	//TODO handle incorect password format

	Server srv(port, argv[2]);
	srv.run_server();

	return 0;
}
