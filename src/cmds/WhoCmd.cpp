#include "ft_irc.hpp"

WhoCommand::WhoCommand(Server *server) : Command(server) {}

WhoCommand::~WhoCommand() {}

void WhoCommand::execute(Client *client, std::vector<std::string> arguments)
{
	std::string channelName = "*";

	if (arguments.empty())
	{
		std::vector<Client *> clients = _server->getServClients();
		for (unsigned long i = 0; i < clients.size(); i++)
			client->reply(RPL_WHOREPLY(client->getNickName(), channelName, clients[i]->getUserName(), clients[i]->getHostName(), this->_server->getServerName(), clients[i]->getNickName(), clients[i]->getRealName()));
	}
	else if (arguments.size() == 1)
	{
		if (arguments[0].at(0) == '#')
		{
			channelName = arguments[0];
			Channel *channel = _server->getChannel(channelName);
			if (channel)
			{
				std::vector<Client *> clients = channel->getChanClients();
				for (unsigned long i = 0; i < clients.size(); i++)
					client->reply(RPL_WHOREPLY(client->getNickName(), channelName, clients[i]->getUserName(), clients[i]->getHostName(), this->_server->getServerName(), clients[i]->getNickName(), clients[i]->getRealName()));
			}
		}
	}
	client->reply(RPL_ENDOFWHO(client->getNickName(), channelName));
	return ;
}
