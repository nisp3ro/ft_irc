#include "IRCBot.hpp"
#include <iostream>
#include <string>
#include <limits>

/**
 * @brief Connects the bot with default configuration settings.
 *
 * Creates and initializes a bot instance with predefined parameters:
 * nickname "Botito", channel "#general", password "1234", 
 * and server at 127.0.0.1:6667. Attempts to connect to the server, 
 * join the specified channel, and start the bot's operation.
 */
void connectDefault() {
    std::string nick = "Botito";
    std::string channel = "#general";
    std::string pass = "1234";
    std::string server_ip = "127.0.0.1";
    int server_port = 6667;

    IRCBot bot(server_ip, server_port, nick, channel, pass);
    if (!bot.connectToServer()) {
        std::cerr << "Could not connect to server.\n";
        return;
    }
    bot.joinChannel();
    bot.run();
}

/**
 * @brief Initializes a bot with user-specified configuration.
 *
 * Interactively prompts the user for configuration details including server IP/port,
 * bot nickname, channel to join, and password. Validates each input and provides
 * appropriate error messages. Allows the user to review and confirm the settings
 * before attempting to connect. If connection fails, offers the opportunity to retry.
 */
void personalizedBot() {
    std::string nick, channel, password;
    std::string server_ip;
    int server_port;

    while (true) {
        std::cout << "Server IP (default: 127.0.0.1): ";
        std::getline(std::cin, server_ip);
        
        if (server_ip.empty()) {
            server_ip = "127.0.0.1";
            break;
        }
        
        if (server_ip == "exit" || server_ip == "quit") {
            return;
        }
        
        if (server_ip.find(" ") != std::string::npos) {
            std::cout << "Do not use \" \" (white spaces)" << std::endl;
            continue;
        }
        break;
    }

    while (true) {
        std::string port_str;
        std::cout << "Server Port (default: 6667): ";
        std::getline(std::cin, port_str);
        
        if (port_str.empty()) {
            server_port = 6667;
            break;
        }
        
        if (port_str == "exit" || port_str == "quit") {
            return;
        }
        
        bool valid = true;
        for (size_t i = 0; i < port_str.size(); i++) {
            if (!isdigit(port_str[i])) {
                valid = false;
                break;
            }
        }
        
        if (!valid) {
            std::cout << "Port must be a number" << std::endl;
            continue;
        }
        
        server_port = atoi(port_str.c_str());
        if (server_port <= 0 || server_port > 65535) {
            std::cout << "Port must be between 1 and 65535" << std::endl;
            continue;
        }
        break;
    }
    
    while (true) {
        std::cout << "Nickname: ";
        std::getline(std::cin, nick);
        
        if (nick.empty()) {
            std::cout << "Nickname cannot be empty" << std::endl;
            continue;
        }
        
        if (nick.find(" ") != std::string::npos) {
            std::cout << "Do not use \" \" (white spaces)" << std::endl; 
            continue;
        }
        
        if (nick == "exit" || nick == "quit") {
            return;
        }

        while (true) {
            std::cout << "Channel (with #): ";
            std::getline(std::cin, channel);
            
            if (channel.empty()) {
                std::cout << "Channel name cannot be empty" << std::endl;
                continue;
            }
            
            if (channel.find(" ") != std::string::npos) {
                std::cout << "Do not use \" \" (white spaces)" << std::endl; 
                continue;
            }
            
            if (channel == "exit" || channel == "quit") {
                return;
            }
            
            if (channel[0] != '#') {
                std::cout << "Channel name must start with \"#\"" << std::endl;
                continue;
            }
            
            if (channel.length() < 2) {
                std::cout << "Channel name must have at least one character after \"#\"" << std::endl;
                continue;
            }
            
            break;
        }

        std::cout << "Password (press Enter if none): ";
        std::getline(std::cin, password);
        
        if (password.find(" ") != std::string::npos) {
            std::cout << "Do not use \" \" (white spaces)" << std::endl; 
            continue;
        }
        if (password == "exit" || password == "quit") {
            return;
        }
        std::cout << std::endl;

        std::cout << "Connection settings:" << std::endl;
        std::cout << "Server: " << server_ip << ":" << server_port << std::endl;
        std::cout << "Nick: " << nick << std::endl;
        std::cout << "Channel: " << channel << std::endl;
        std::cout << "Password: " << (password.empty() ? "(none)" : password) << std::endl;

        std::string choice;
        std::cout << "Are these correct? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
        if (choice == "n")
            continue;
            
        IRCBot bot(server_ip, server_port, nick, channel, password);
        if (!bot.connectToServer()) {
            std::cout << "Connection failed. Try again? (y/n): ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (choice != "y")
                return;
            continue;
        }
        
        bot.joinChannel();
        bot.run();
    }
}

/**
 * @brief Main entry point for the IRC bot application.
 *
 * Initializes the random number generator for the bot's response selection.
 * Displays a welcome message and presents a menu for the user to choose 
 * between using default settings, customizing the bot, or exiting the program.
 * 
 * @return int Returns 0 on successful execution.
 */
int main() {
    std::srand(std::time(NULL));
    std::cout << "Welcome to the amazing useless bot v1.0! 🤖🔥" << std::endl;
    std::string choice;

    while (true) {
        std::cout << "\nSelect an option:" << std::endl;
        std::cout << "1: standard useless bot" << std::endl;
        std::cout << "2: personalized useless bot" << std::endl;
        std::cout << "3: exit" << std::endl;
        std::cout << "> ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == "1") {
            connectDefault();
        } 
        else if (choice == "2") {
            personalizedBot();
        }
        else if (choice == "3" || choice == "exit" || choice == "quit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        else {
            std::cout << "Please choose 1, 2, or 3..." << std::endl;
        }
    }

    return 0;
}