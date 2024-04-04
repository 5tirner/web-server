TARGET  = httpserver

CC      = c++ -Wall -Wextra -Werror -std=c++98 -fsanitize=address  -g3

OBJDIR  = obj

SRC     = main.cpp\
			./configFileAndPars/pars.cpp\
			./configFileAndPars/openFile.cpp\
			./configFileAndPars/selectServers.cpp\
			./configFileAndPars/serverInfos.cpp\
			./configFileAndPars/checkValues.cpp\
			./multuplexing/endPointConnection.cpp\
			./multuplexing/incomingData.cpp \
			./methods/post.cpp\
			./methods/get.cpp \
			./methods/delete.cpp \
			./methods/ErrorPages.cpp \
			./multuplexing/sendResponse.cpp\
			./commonGateway/cgi.cpp

OBJ     = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

clean:
	@rm -f upload/* .cgi_file*
	@rm -f $(OBJ)

fclean: clean
	@rm -fr $(TARGET) $(OBJDIR)

re: fclean $(TARGET)

git: fclean
	@git add .
	@git commit -m "@ysabr try to fix the problem of ../"
	@git push origin cgi-feature
	@echo "Pushed to cgi-feature"

.PHONY: clean fclean re all git