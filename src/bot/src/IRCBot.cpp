#include "IRCBot.hpp"

/**
 * @brief Constructor for the IRCBot class.
 *
 * Initializes a new IRCBot instance with the specified server details, nickname,
 * channel, and password for connection.
 *
 * @param ip The IP address of the IRC server to connect to.
 * @param port The port number of the IRC server.
 * @param nick The nickname for the bot to use on the IRC server.
 * @param chan The channel name for the bot to join.
 * @param pass The password for authentication with the IRC server.
 */
IRCBot::IRCBot(const std::string& ip, int port, const std::string& nick, const std::string& chan, const std::string& pass)
    : server_ip(ip), server_port(port), nickname(nick), channel(chan), sockfd(-1), password(pass), running(false) {}

/**
 * @brief Destructor for the IRCBot class.
 *
 * Ensures that the socket connection is properly closed when the bot is destroyed.
 */
IRCBot::~IRCBot() {
    if (sockfd != -1)
        close(sockfd);
}

/**
 * @brief Establishes a connection to the IRC server.
 *
 * Creates a socket and attempts to connect to the IRC server using the
 * IP address and port specified during initialization.
 *
 * @return bool Returns true if connection was successful, false otherwise.
 */
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

/**
 * @brief Sends a raw message to the IRC server.
 *
 * Appends the necessary CR+LF to the message and sends it to the IRC server
 * through the established socket connection.
 *
 * @param msg The message to be sent to the server.
 */
void IRCBot::sendRaw(const std::string& msg) {
    std::string message = msg + "\r\n";
    send(sockfd, message.c_str(), message.length(), 0);
}

/**
 * @brief Registers with the IRC server and joins the specified channel.
 *
 * Sends the necessary NICK, USER, and PASS commands to authenticate with the server.
 * After successful registration, joins the channel specified during initialization.
 * Handles various registration responses from the server.
 */
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

/**
 * @brief Handles PING requests from the IRC server.
 *
 * Detects PING messages from the server and responds with appropriate PONG messages
 * to maintain the connection and prevent timeouts.
 *
 * @param msg The message received from the server to check for PING requests.
 */
void IRCBot::handlePing(const std::string& msg) {
    size_t pos = msg.find("PING :");
    if (pos != std::string::npos) {
        std::string response = "PONG :" + msg.substr(pos + 6);
        sendRaw(response);
    }
}

/**
 * @brief Responds to messages in the channel.
 *
 * Checks if the received message is a PRIVMSG directed to the channel the bot is in.
 * If it is, sends a random response to the channel.
 *
 * @param msg The message received from the server to respond to.
 */
void IRCBot::respondToMessage(const std::string& msg) {
    if (msg.find("PRIVMSG") != std::string::npos && msg.find(channel) != std::string::npos) {
        sendRaw("PRIVMSG " + channel + chooseResponse());
    }
}

/**
 * @brief Selects a random response from a predefined list of programmer jokes.
 *
 * Generates a random number and returns one of several programming-related jokes
 * to be used when responding to channel messages.
 *
 * @return std::string A randomly selected joke or message to send to the channel.
 */
std::string IRCBot::chooseResponse() {
    int ran = std::rand() % 10;
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
        case 9: return " : Segmentation fault (core dumped). haha, no";
    }
    return "Null";
}

/**
 * @brief Checks if an exit command has been received.
 *
 * Examines the given message for exit commands like "!exit" or "!quit",
 * either from the IRC server or directed specifically to the bot.
 *
 * @param msg The message to check for exit commands.
 * @return bool Returns true if an exit command is detected, false otherwise.
 */
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

/**
 * @brief Stops the bot's operation.
 *
 * Sets the running flag to false and closes the socket connection to the IRC server.
 */
void IRCBot::stop() {
    running = false;
    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }
}

/**
 * @brief Main execution loop for the bot.
 *
 * Continuously monitors both the IRC socket and standard input for messages.
 * Processes incoming server messages, responds to channel messages, and handles
 * commands from the console. Runs until an exit command is detected or the 
 * server disconnects.
 */
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


