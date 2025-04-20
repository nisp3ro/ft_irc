############################################################################################################
# Variables
NAME        = IRCBot
DEBUG       = 0

# Dependencies
INCLUDE     = include

# Sources
SRCS        = main.cpp IRCBot.cpp

# Objects
OBJDIR      = objs
OBJS        = $(SRCS:%.cpp=$(OBJDIR)/%.o)

# Compiler
CC          = c++
CFLAGS      = -Wall -Wextra -Werror -std=c++98
RM          = rm -rdf

# Colors
GREEN       = \033[0;32m
UGREEN      = \033[4;32m
RED         = \033[0;31m
YELLOW      = \033[0;33m
LILLY       = \033[0;35m
CYAN        = \033[0;36m
NC          = \033[0m # No color

############################################################################################################
# IRCBot Rules

all: ascii_art

ascii_art:
	@if ! $(MAKE) -q $(NAME); then \
		echo "\n$(CYAN)██╗██████╗  ██████╗██████╗  ██████╗ ████████╗"; \
		echo "██║██╔══██╗██╔════╝██╔══██╗██╔═══██╗╚══██╔══╝"; \
		echo "██║██████╔╝██║     ██████╔╝██║   ██║   ██║"; \
		echo "██║██╔══██╗██║     ██╔══██╗██║   ██║   ██║"; \
		echo "██║██║  ██║╚██████╗██████╔╝╚██████╔╝   ██║"; \
		echo "╚═╝╚═╝  ╚═╝ ╚═════╝╚═════╝  ╚═════╝    ╚═╝"; \
		printf "          IRC BOT ASSISTANT\033[0m\n"; \
		echo "$(YELLOW)\nCreating bot program...$(GREEN)"; \
		i=0; \
		while [ $$i -lt 15 ]; do \
			echo -n "█"; \
			sleep 0.05; \
			i=$$((i + 1)); \
		done; \
		$(MAKE) -s $(NAME); \
	else \
		echo "$(GREEN)[$(NAME)] is already up to date.$(NC)"; \
	fi

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo -n "█"
	@$(CC) $(CFLAGS) -I$(INCLUDE) -D DEBUG=$(DEBUG) -c $< -o $@

$(NAME): $(OBJS)
	@printf "$(NC)"
	@$(CC) $(CFLAGS) -I$(INCLUDE) $(OBJS) -o $(NAME) && \
	(printf "$(UGREEN)\n%s$(NC)" "[$(NAME)]"; printf "$(GREEN)%s$(NC)\n" "Compiled successfully.")

clean:
	@$(RM) $(OBJDIR)
	@printf "$(RED)%s$(NC)\n" "[IRCBot] Object files cleaned."

fclean: clean
	@$(RM) $(NAME)
	@printf "$(RED)%s$(NC)\n" "[IRCBot] Cleaned successfully."

re: fclean all

.PHONY: all clean fclean re
############################################################################################################
