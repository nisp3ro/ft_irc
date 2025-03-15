#ifndef COMMAND_HANDLER_CLASS_H
# define COMMAND_HANDLER_CLASS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

class Server;
class Command;
class Client;

class CommandHandler
{
private:
	Server *_server;
	std::map<std::string, Command *> _commands;
public:
	CommandHandler(Server *server);
	~CommandHandler();

	void invoke(Client *client, std::string const &message);
};

#endif
