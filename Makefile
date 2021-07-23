# Colors
# ****************************************************************************

_GREY	= \033[30m
_RED	= \033[31m
_GREEN	= \033[32m
_YELLOW	= \033[33m
_BLUE	= \033[34m
_PURPLE	= \033[35m
_CYAN	= \033[36m
_WHITE	= \033[37m
_END	= \033[0m

# ****************************************************************************

NAME = webserv

# Config
# ****************************************************************************

SHELL = /bin/bash
CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -I $(SRC_DIR)
DEBUG_FLAGS = -g3

# Source files
# ****************************************************************************

SRC_DIR = srcs/
OBJ_DIR = objs/

SRCS = $(shell find srcs -name '*.cpp' | sed 's!^.*/!!')
OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))

# Recipe
# ****************************************************************************

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(_END)\nCompiled source files\n"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	@printf "$(_GREEN)Finish compiling $@!\n"
	@printf "Try \"./$@\" to use$(_END)\n"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@if [ ! -d $(OBJ_DIR) ];then mkdir $(OBJ_DIR); fi
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@printf "$(_GREEN)█"

clean:
	@printf "$(_YELLOW)Removing object files ...$(_END)\n"
	@rm -rf $(OBJ_DIR) *.dSYM

fclean:
	@printf "$(_RED)Removing object files and program ...$(_END)\n"
	@rm -rf $(NAME) $(OBJ_DIR) *.dSYM

re: fclean all

debug: CXXFLAGS += -fsanitize=address $(DEBUG_FLAGS)
debug: re
	@printf "$(_BLUE)Debug build done$(_END)\n"

leak: CXXFLAGS += $(DEBUG_FLAGS)
leak: re
	@printf "$(_BLUE)Leak check build done$(_END)\n"

check:
	cd test && ./test.sh

PHONY: all clean fclean re debug leak check
