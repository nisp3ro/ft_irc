#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <ctime>

class IRCBot {
    private:
        std::string server_ip;
        int server_port;
        std::string nickname;
        std::string channel;
        int sockfd;
        std::string password;

    public:

        IRCBot(std::string ip, int port, std::string nick, std::string chan, std::string pass);
        ~IRCBot();
        //copia y operator= que coñazo mi pana
            
        bool connectToServer();
        void sendRaw(std::string msg);
        void joinChannel();
        void handlePing(const std::string& msg);
        void respondToMessage(const std::string& msg);
        std::string chooseResponse();
        void run();
};
