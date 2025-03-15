#include "ft_irc.hpp"

PongCommand::PongCommand(Server *server) : Command(server) {}

PongCommand::~PongCommand() {}

void PongCommand::execute(Client *client, std::vector<std::string> arguments)
{

	if (arguments.empty()) {
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "PONG"));
		return;
	}

	client->write(RPL_PING(client->getPrefix(), arguments.at(0)));
}
