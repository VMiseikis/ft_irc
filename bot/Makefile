NAME = loveBot

SRC = main.cpp loveBot.cpp

SRC_D = src/

OBJ_D = obj/

OBJ = ${patsubst ${SRC_D}%.cpp, ${OBJ_D}%.o, ${addprefix ${SRC_D}, ${SRC}}} 

INC = inc/

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

# -pedantic -g -fsanitize=address

all: ${NAME}

${OBJ_D}%.o: ${SRC_D}%.cpp
	@mkdir -p ./obj
	@${CC} -I${INC} ${CFLAGS} $^ -c -o $@

${NAME}: ${OBJ}
	@${CC} ${CFLAGS} $^ -o ${NAME}
	@echo "\033[1;35mloveBot is ready and eager to be used!\033[1;0m"

clean:
	@rm -rf ${OBJ_D}
	@echo "\033[1;35mCleaned up!\033[1;0m"

fclean: clean
	@rm -fr ${NAME}
	@echo "\033[1;35mloveBot erased!\033[1;0m"

re: fclean all

.PHONY: all clean fclean re
