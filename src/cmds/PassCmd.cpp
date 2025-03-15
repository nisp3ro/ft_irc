#include "ft_irc.hpp"

PassCommand::PassCommand(Server *server, bool auth) : Command(server, auth) {}

PassCommand::~PassCommand() {}

void PassCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (client->isRegistered())
	{
		client->reply(ERR_ALREADYREGISTERED(client->getPrefix()));
		return;
	}

	if (arguments.empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getPrefix(), "PASS"));
		return;
	}

	if (_server->getPassword() != arguments[0].substr(arguments[0][0] == ':' ? 1 : 0))
	{
		client->reply(ERR_PASSWDMISMATCH(client->getPrefix()));
		return;
	}

	client->setCorrectPassword(true);
	client->welcome();
}
