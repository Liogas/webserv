CXX 		= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g
NAME		= webserv

SRCS		= main.cpp ParseConfig.cpp ServerConfig.cpp Request.cpp Client.cpp Server.cpp Route.cpp utils.cpp
OBJS		= $(SRCS:.cpp=.o)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -o $@ -c $^

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

run: $(NAME)
	@./$(NAME)

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -rf $(NAME)

re : fclean $(NAME)

.PHONY: clean fclean re