TARGET  = a.out

CC      = c++ -Wall -Wextra -Werror  -std=c++98 -fsanitize=address -g3

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
		  ./multuplexing/sendResponse.cpp

OBJ     = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f upload/*
	@rm -fr $(TARGET) $(OBJDIR)

re: fclean $(TARGET)

git: fclean
	@git add .
	@git commit -m "after the merge with 5triner"
	@git push origin merged
	@echo "Pushed to merged"

.PHONY: clean fclean re all git