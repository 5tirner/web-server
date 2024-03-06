# SRCS	= main.cpp\
# 		./configFileAndPars/pars.cpp\
# 		./configFileAndPars/openFile.cpp\
# 		./configFileAndPars/selectServers.cpp\
# 		./configFileAndPars/serverInfos.cpp\
# 		./configFileAndPars/checkValues.cpp\
# 		./multuplexing/endPointConnection.cpp\
# 		./multuplexing/incomingData.cpp \
# 		./methods/post.cpp

# OBJS	= $(SRCS:.cpp=.o)

# rm		= rm -rf

# NAME	= httpserver

# CXX		= c++

# CXXFLAGS = -Wall -Wextra -Werror  -std=c++98 -fsanitize=address -g3


# all : $(NAME)

# $(NAME) : $(OBJS)
# 	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# clean :
# 	$(RM) $(OBJS)

# fclean : clean
# 	@rm -fr ./upload/*
# 	$(RM) $(NAME)

# re : fclean all

# git: fclean
# 	git add .
# 	git commit -m "yachaab handling chunked post request: fix chunk request '\0' problem"
# 	git push origin merged

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
		./methods/post.cpp

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
	@rm -f data/*
	@git add .
	@git commit -m "PARSSING CLIENT POST REQUEST DONE : start implimanating extension logic"
	@git push origin merged
	@echo "Pushed to merged"

.PHONY: clean fclean re all git