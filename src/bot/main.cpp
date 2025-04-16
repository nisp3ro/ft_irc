#include "IRCBot.hpp"
#include <iostream>
#include <string>
#include <limits>


void personalizedBot(){
    std::string nick, channel, password;
    while(true){
            std::cout << "Nickname: ";
            std::getline(std::cin, nick);
            if((nick.find(" ") != std::string::npos)){
                std::cout << "Do not use \" \" (white spaces)" << std::endl; 
                continue;
            }

            while (true){
                std::cout << "Channel (with #): ";
                std::getline(std::cin, channel);
                if((channel.find(" ") != std::string::npos)){
                    std::cout << "Do not use \" \" (white spaces)" << std::endl; 
                    continue;
                }
                if(!channel.find("#", 0))
                    break;
                std::cout << "Do not forget to use \"#\"" << std::endl;
            }

            std::cout << "Password (press Enter if none): ";
            std::getline(std::cin, password);
            if((password.find(" ") != std::string::npos)){
                std::cout << "Do not use \" \" (white spaces)" << std::endl; 
                continue;
            }
            std::cout << std::endl;

            std::cout << "Nick: " << nick << std::endl;
            std::cout << "Channel: " << channel << std::endl;
            std::cout << "Password: " << password << std::endl;

            std::string choice;
            std::cout << "Are these correct? y/n" << std::endl;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
            if (choice == "n")
                continue;
            IRCBot bot("127.0.0.1", 6667, nick, channel, password);
            if (!bot.connectToServer()) {
                continue;
            }
            bot.joinChannel();
            bot.run();
        }
}

int main() {
    std::srand(std::time(NULL));
    std::cout << "Welcome to the amazing useless bot v1.0! 🤖🔥" << std::endl;
    std::string choice;

    while(true){
        std::cout << "Select an useless option:" << std::endl;
        std::cout << "1: standard useless bot" << std::endl;
        std::cout << "2: personalized useless bot" << std::endl;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == "1") {
            std::string nick = "Botito";
            std::string channel = "#general";
            std::string pass = "1234";

            IRCBot bot("127.0.0.1", 6667, nick, channel, pass);
            if (!bot.connectToServer()) {
                std::cerr << "No se pudo conectar al servidor.\n";
                return 1;
            }
            bot.joinChannel();
            bot.run();
        } else if (choice == "2")
            personalizedBot();
        else
            std::cout << "Just choose 1 or 2... are you more useless than this bot?" << std::endl;
    }

    return 0;
}