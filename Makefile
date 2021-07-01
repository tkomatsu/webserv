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

.SHELL = /bin/bash
CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -I $(SRC_DIR)
DEBUG_CXXFLAGS = -g3

# Source files
# ****************************************************************************

SRC_DIR = srcs/
OBJ_DIR = objs/

# prefix

SRCS = $(shell find srcs -name '*.cpp' | sed 's!^.*/!!')
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)%.o)

# Recipe
# ****************************************************************************

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(_END)\nCompiled source files"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	@echo "$(_GREEN)Finish compiling $@!"
	@echo "Try \"./$@\" to use$(_END)"

$(OBJS): $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@printf "$(_GREEN)█"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo "$(_YELLOW)Removing object files ...$(_END)"
	@rm -rf $(OBJ_DIR)
	@rm -fr *.dSYM

fclean:
	@echo "$(_RED)Removing object files and program ...$(_END)"
	@rm -rf $(NAME) $(OBJ_DIR)
	@rm -fr *.dSYM expected actual

re: fclean all

debug: CXXFLAGS += -fsanitize=address $(DEBUG_CXXFLAGS)
debug: re
	@echo "$(_BLUE)Debug build done$(_END)"

leak: CXXFLAGS += $(DEBUG_CXXFLAGS)
leak: re
	@echo "$(_BLUE)Leak check build done$(_END)"

test: re
	./$(NAME)

PHONY: all clean fclean re debug leak test
