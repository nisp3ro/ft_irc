#ifndef IRCBOT_HPP
# define IRCBOT_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <cstdlib>
# include <ctime>
# include <cerrno>
# include <cstdio>
# include <sys/select.h>
# include <netdb.h>

class IRCBot {
    private:
        std::string server_ip;
        int server_port;
        std::string nickname;
        std::string channel;
        int sockfd;
        std::string password;
        bool running;

    public:
        IRCBot(const std::string& ip, int port, const std::string& nick, const std::string& chan, const std::string& pass);
        ~IRCBot();
            
        bool connectToServer();
        void sendRaw(const std::string& msg);
        void joinChannel();
        void handlePing(const std::string& msg);
        void respondToMessage(const std::string& msg);
        std::string chooseResponse();
        void run();
        void stop();
        bool checkExitCommand(const std::string& msg);
};

#endif
