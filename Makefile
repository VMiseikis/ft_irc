NAME = ircserv

SRC = main.cpp server.cpp client.cpp channel.cpp commands.cpp

SRC_D = src/

OBJ_D = obj/

OBJ = ${patsubst ${SRC_D}%.cpp, ${OBJ_D}%.o, ${addprefix ${SRC_D}, ${SRC}}}

INC = inc/

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

all: ${NAME}

${OBJ_D}%.o: ${SRC_D}%.cpp
	@mkdir -p ./obj
	@${CC} -I${INC} ${CFLAGS} $^ -c -o $@

${NAME}: ${OBJ}
	@${CC} ${CFLAGS} $^ -o ${NAME}
	@echo "\033[1;32mircserv is ready.\033[1;0m"

clean:
	@rm -rf ${OBJ_D} 
	@echo "\033[1;31mObject files removed.\033[1;0m"
fclean: clean
	@rm -fr ${NAME}
	@echo "\033[1;31mircserv removed.\033[1;0m"
re: fclean all

.PHONY: all re clean fclean
