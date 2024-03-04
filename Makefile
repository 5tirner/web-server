SRCS	= main.cpp\
		./configFileAndPars/pars.cpp\
		./configFileAndPars/openFile.cpp\
		./configFileAndPars/selectServers.cpp\
		./configFileAndPars/serverInfos.cpp\
		./configFileAndPars/checkValues.cpp\
		./multuplexing/endPointConnection.cpp\
<<<<<<< HEAD
		./methods/get.cpp\
		./methods/post.cpp\
		./methods/delete.cpp\
		./commonGateway/cgi.cpp\
=======
		./multuplexing/incomingData.cpp \
		./methods/post.cpp \
		# ./methods/get.cpp\
		# ./methods/post.cpp\
		# ./methods/delete.cpp\
>>>>>>> yachaab

OBJS	= $(SRCS:.cpp=.o)

rm		= rm -rf

NAME	= httpserver

CXX		= c++

CXXFLAGS = -Wall -Wextra -Werror  -std=c++98 -fsanitize=address -g3


all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean :
	$(RM) $(OBJS)

fclean : clean
	@rm -fr ./data/*
	$(RM) $(NAME)

re : fclean all

git: fclean
	git add .
	git commit -m "yachaab handling chunked post request: add some error checks"
	git push origin yachaab