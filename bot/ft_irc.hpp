#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#define RED_C 	"\033[1;31m"
#define GREEN 	"\033[1;32m"
#define YELLOW 	"\033[1;33m"
#define BLUE 	"\033[1;34m"
#define MAGENTA	"\033[1;35m"
#define CYAN 	"\033[1;36m"
#define RESET	"\033[1;0m"

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>


#include <cstring>
#include <sstream>
#include <iostream>
#include <poll.h>
#include <vector>
#include <map>

class	Client;
class	Server;
class	Channel;

#endif
