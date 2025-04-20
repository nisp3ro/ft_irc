#include "IRCBot.hpp"

IRCBot::IRCBot(std::string ip, int port, std::string nick, std::string chan, std::string pass)
    : server_ip(ip), server_port(port), nickname(nick), channel(chan), sockfd(-1), password(pass), running(false) {}

IRCBot::~IRCBot() {
    if (sockfd != -1)
        close(sockfd);
}

bool IRCBot::connectToServer() {
    std::cout << "Connecting to " << server_ip << ":" << server_port << std::endl;

    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation error\n";
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error connecting to IRC\n";
        close(sockfd);
        sockfd = -1;
        return false;
    }

    return true;
}

void IRCBot::sendRaw(std::string msg) {
    msg += "\r\n";
    send(sockfd, msg.c_str(), msg.length(), 0);
}

void IRCBot::joinChannel() {
    std::cout << "Registering bot..." << std::endl;

    sendRaw("NICK " + nickname);
    sendRaw("USER " + nickname + " 0 * :" + nickname);
    sendRaw("PASS " + password);

    bool registered = false;
    char buffer[512];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;

        std::string msg(buffer);
        std::cout << ">> " << msg;

        handlePing(msg);

        if (msg.find(" 001 ") != std::string::npos) {
            std::cout << "Success.\n";
            registered = true;
            break;
        }

        if (msg.find(" 433 ") != std::string::npos) {
            std::cout << "Nick already in use.\n";
            break;
        }

        if (msg.find(" 464 ") != std::string::npos) {
            std::cout << "Incorrect password.\n";
            break;
        }
    }

    if (registered) {
        sendRaw("JOIN " + channel);
        std::cout << "Joined to " << channel << std::endl;
    } else {
        std::cout << "Error connecting.\n";
    }
}

void IRCBot::handlePing(const std::string& msg) {
    size_t pos = msg.find("PING :");
    if (pos != std::string::npos) {
        std::string response = "PONG :" + msg.substr(pos + 6);
        sendRaw(response);
    }
}

void IRCBot::respondToMessage(const std::string& msg) {
    if (msg.find("PRIVMSG") != std::string::npos && msg.find(channel) != std::string::npos) {
        sendRaw("PRIVMSG " + channel + chooseResponse());
    }
}

std::string IRCBot::chooseResponse() {
    int ran = std::rand() % 10;
    std::string resp;
    switch (ran) {
        case 0: return " : Why do programmers prefer dark mode? Because light attracts bugs!";
        case 1: return " : There are only 10 kinds of people in this world: those who understand binary and those who don't.";
        case 2: return " : A SQL statement walks into a bar and sees two tables. It approaches and asks, 'Can I join you?'";
        case 3: return " : Why did the programmer quit his job? Because he didn't get arrays.";
        case 4: return " : How many programmers does it take to change a light bulb? None, that's a hardware problem.";
        case 5: return " : Why do Java developers wear glasses? Because they can't C#!";
        case 6: return " : I would tell you a UDP joke, but you might not get it.";
        case 7: return " : Debugging: Being the detective in a crime movie where you're also the murderer.";
        case 8: return " : My code doesn't work, I have no idea why. My code works, I have no idea why.";
        case 9: return " : Segmentation fault (core dumped). que no tonto";
    }
    return "Null";
}

bool IRCBot::checkExitCommand(const std::string& msg) {
    if (msg.find("!exit") != std::string::npos || 
        msg.find("!quit") != std::string::npos) {
        std::cout << "Exit command received. Shutting down bot...\n";
        return true;
    }
    
    if (msg.find("PRIVMSG") != std::string::npos && 
        msg.find(nickname) != std::string::npos &&
        (msg.find(":!exit") != std::string::npos || 
         msg.find(":!quit") != std::string::npos)) {
        std::cout << "Exit command received from IRC. Shutting down bot...\n";
        return true;
    }
    
    return false;
}

void IRCBot::stop() {
    running = false;
    if (sockfd != -1) {
        sendRaw("QUIT :Bot shutting down");
        close(sockfd);
        sockfd = -1;
    }
}

void IRCBot::run() {
    running = true;
    char buffer[512];
    
    std::cout << "Bot is running. Type !exit or !quit to exit.\n";
    
    fd_set read_fds;
    struct timeval tv;
    
    while (running) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        
        tv.tv_sec = 0;
        tv.tv_usec = 500000;
        
        int activity = select(sockfd + 1, &read_fds, NULL, NULL, &tv);
        
        if (activity < 0 && errno != EINTR) {
            std::cerr << "Select error\n";
            break;
        }
        
        if (FD_ISSET(sockfd, &read_fds)) {
            memset(buffer, 0, sizeof(buffer));
            int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            
            if (n <= 0) {
                std::cout << "Server disconnected.\n";
                break;
            }
            
            std::string msg(buffer);
            std::cout << ">> " << msg;
            
            handlePing(msg);
            respondToMessage(msg);
            
            if (checkExitCommand(msg)) {
                stop();
                break;
            }
        }
        
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, sizeof(buffer) - 1, stdin) != NULL) {
                std::string input(buffer);
                
                if (checkExitCommand(input)) {
                    stop();
                    break;
                }
                
                if (input.length() > 1 && input[0] != '!') {
                    input.erase(input.find_last_not_of("\r\n") + 1);
                    sendRaw("PRIVMSG " + channel + " :" + input);
                }
            }
        }
    }

    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }
}


