SRCS	= main.cpp\
		./configFileAndPars/pars.cpp\
		./configFileAndPars/openFile.cpp\
		./configFileAndPars/selectServers.cpp\
		./configFileAndPars/serverInfos.cpp\
		./configFileAndPars/checkValues.cpp\
		./Server/Server.cpp\
		./Server/HTTPRequest.cpp\
		./Methods/POST.cpp\
		./Methods/GET.cpp\
		./Methods/DELETE.cpp\

OBJS	= $(SRCS:.cpp=.o)

rm		= rm -rf

NAME	= server

CXX		= c++

CXXFLAGS = -Wall -Wextra -Werror  -std=c++98 -fsanitize=address -g3

all : $(NAME)

$(NAME) : $(OBJS)

	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean :

	$(RM) $(OBJS)

fclean : clean

	$(RM) $(NAME)

re : fclean all