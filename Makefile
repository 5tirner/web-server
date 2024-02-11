SRCS	= main.cpp\
		./configFileAndPars/pars.cpp\
		./configFileAndPars/openFile.cpp\
		./configFileAndPars/selectServers.cpp\
		./configFileAndPars/serverInfos.cpp\
		./configFileAndPars/checkValues.cpp\
		./multuplexing/endPointConnection.cpp\
		./methods/get.cpp\
		./methods/post.cpp\
		./methods/delete.cpp\
		./Server/Server.cpp\
		./Server/HTTPRequest.cpp\
		

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