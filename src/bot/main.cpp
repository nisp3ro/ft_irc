#include "IRCBot.hpp"
#include <iostream>
#include <string>
#include <limits>


int main() {
    std::srand(std::time(NULL));
    std::string nick, channel, password;

    std::cout << "Welcome to the most fucking amazing bot ever chavaloide! 🤖🔥" << std::endl;

    while(true){
        std::cout << "Nickname: ";
        std::getline(std::cin, nick);

        while (true){
            std::cout << "Channel (with #): ";
            std::getline(std::cin, channel);
            if(!channel.find("#", 0))
                break;
        }

        std::cout << "Password (press Enter if none): ";
        std::getline(std::cin, password);
        std::cout << std::endl;

        std::cout << "Nick: " << nick << std::endl;
        std::cout << "Channel: " << channel << std::endl;
        std::cout << "Password: " << password << std::endl;

        std::string choice;
        std::cout << "Are these correct? y/n" << std::endl;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
        if (choice == "n")
            break;
        IRCBot bot("127.0.0.1", 6667, nick, channel, password);
        if (!bot.connectToServer()) {
            continue;
        }
        bot.joinChannel();
        bot.run();
    }
    return 0;
}