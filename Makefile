############################################################################################################
# Variables
NAME		=		ircserv
DEBUG		=		0

# Dependencies	
INC_DIR		=		include
INC         =       $(addprefix $(INC_DIR)/, \
					Channel.hpp Client.hpp Command.hpp CommandHandler.hpp ft_irc.hp Replies.hpp Server.hpp )

# Sources
SRC_DIR		=		src
SRCS		=		$(addprefix $(SRC_DIR)/, \
					Channel.cpp Client.cpp CommandHandler.cpp main.cpp Server.cpp utils.cpp \
                    cmds/InvitCmd.cpp cmds/JoinCmd.cpp cmds/KickCmd.cpp cmds/ListCmd.cpp cmds/ModeCmd.cpp \
                    cmds/NickCmd.cpp cmds/NoticeCmd.cpp cmds/PartCmd.cpp cmds/PassCmd.cpp cmds/PingCmd.cpp \
                    cmds/PongCmd.cpp cmds/PrivMsgCmd.cpp cmds/QuitCmd.cpp cmds/UserCmd.cpp cmds/WhoCmd.cpp \
					cmds/TopicCmd.cpp )

#blinker

BLINK		=		\001\033[1;35;5m\002

# Objects
OBJ_DIR		=		obj
OBJS		=		$(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Compiler
CC			=		c++
CFLAGS		=		-Wall -Wextra -Werror -std=c++98

# Colors
GREEN		=		\033[0;32m
UGREEN		=		\033[4;32m
RED			=		\033[0;31m
YELLOW		=		\033[0;33m
LILLY		=		\033[0;35m
NC			=		\033[0m # No color

############################################################################################################
# Minishell Rules

all: ascii_art

ascii_art:
	@if ! $(MAKE) -q $(NAME); then \
		echo "\n$(BLINK))███████╗████████╗     ██╗██████╗  ██████╗"; \
		echo "██╔════╝╚══██╔══╝     ██║██╔══██╗██╔════╝"; \
		echo "█████╗     ██║        ██║██████╔╝██║"; \
		echo "██╔══╝     ██║        ██║██╔══██╗██║"; \
		echo "██║        ██║███████╗██║██║  ██║╚██████╗"; \
		echo "╚═╝        ╚═╝╚══════╝╚═╝╚═╝  ╚═╝ ╚═════╝"; \
		printf "          BY JVIDAL-T , JAINAVAS MRUBAL-C\033[0m\n"; \
		echo "$(YELLOW)\nCreating program...$(GREEN)"; \
		i=0; \
		while [ $$i -lt 20 ]; do \
			echo -n "█"; \
			sleep 0.05; \
			i=$$((i + 1)); \
		done; \
		$(MAKE) -s $(NAME); \
	else \
		echo "$(GREEN)[$(NAME)] is already up to date.$(NC)"; \
	fi

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -n "█"
	@$(CC) $(CFLAGS) -I$(INC_DIR) -D DEBUG=$(DEBUG) -c $< -o $@

$(NAME): $(OBJS) $(OBJ_DIR)
	@printf "$(NC)"
	@$(CC) $(CFLAGS) -I$(INC_DIR) $(OBJS) -o $(NAME) && \
	(printf "$(UGREEN)\n%s$(NC)" "[$(NAME)]"; printf "$(GREEN)%s$(NC)\n" "Compiled successfully.")

clean:
	@rm -rdf $(OBJ_DIR)
	@printf "$(RED)%s$(NC)\n" "[ft_irc] Object files cleaned."

fclean: clean
	@rm -f $(NAME)
	@printf "$(RED)%s$(NC)\n" "[ft_irc] Cleaned successfully."

re:	fclean all

debug: fclean
	$(eval DEBUG=1)
	$(MAKE) all DEBUG=1 -s

.PHONY:	all clean fclean re debugflags debug
############################################################################################################
