SRCS	= main.cpp\
		pars.cpp\
		get.cpp\
		post.cpp\
		delete.cpp\
		openFile.cpp\
		selectServers.cpp\
		serverInfos.cpp\

OBJS	= $(SRCS:.cpp=.o)

rm		= rm -rf

NAME	= httpserver

CXX		= c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

all : $(NAME)

$(NAME) : $(OBJS)

	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean :

	$(RM) $(OBJS)

fclean : clean

	$(RM) $(NAME)

re : fclean all