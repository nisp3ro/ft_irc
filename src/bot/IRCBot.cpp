#include "IRCBot.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>

IRCBot::IRCBot(std::string ip, int port, std::string nick, std::string chan, std::string pass)
    : server_ip(ip), server_port(port), nickname(nick), channel(chan), sockfd(-1), password(pass) {}

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
    switch (ran){
        case 0: return resp = " : Hola! Paso de tu puta cara pringao 😄 Viva Españita!";
        case 1: return resp = " : Tómate un cafelito con ellas, anda.";
        case 2: return resp = " : Cuéntaselo a mi pana Lucacu ya verás cómo le importa más que a mi.";
        case 3: return resp = " : Alberto sacó un 100 en el final exam bro.";
        case 4: return resp = " : Jose no tiene papeles.";
        case 5: return resp = " : El tucumano se trinca la mamasota";
        case 6: return resp = " : No autistas por favor.";
        case 7: return resp = " : So... no head?";
        case 8: return resp = " : No me mandes a un perro.";
        case 9: return resp = " : Segmentation fault (core dumped). que no tonto";
    }
    return resp = "Null";
}

void IRCBot::run() {
    char buffer[512];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;

        std::string msg(buffer);
        std::cout << ">> " << msg;

        handlePing(msg);
        respondToMessage(msg);
    }

    close(sockfd);
}


