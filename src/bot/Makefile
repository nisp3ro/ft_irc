NAME        = IRCBot
SRCS        = main.cpp IRCBot.cpp
INCLUDE     = include
OBJDIR      = objs
OBJS        = $(SRCS:%.cpp=$(OBJDIR)/%.o)
CC          = c++ -Wall -Wextra -Werror -std=c++98
RM          = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) -I$(INCLUDE) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
