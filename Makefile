NAME = webserv
CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
SRCS = $(shell find . -name '*.cpp')
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(OBJS)
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re