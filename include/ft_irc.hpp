#ifndef IRC_H
# define IRC_H

# ifndef DEBUG
#  define DEBUG 0
# endif

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 8192
# endif

# define TRUE 1
# define FALSE 0

# include "Client.hpp"
# include "Channel.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"
# include "Command.hpp"
# include "Replies.hpp"

// utils.cpp
std::string					ft_inet_ntop6(const void *a0);
std::vector<std::string>	ft_split(const std::string& str, char c);
std::string					dateString(void);
std::string					intToString(int num);
bool						containsOnlyDigits(const std::string &str);

#endif
