#include "ft_irc.hpp"

ListCommand::ListCommand(Server *server) : Command(server) {};

ListCommand::~ListCommand() {};

bool isInChannelsList(Channel *channel, std::vector<std::string> channelNames)
{
	for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		if (channel->getName() == *it)
			return true;
	}
	return false;
}

void ListCommand::execute(Client *client, std::vector<std::string> arguments)
{
	std::vector<Channel *> chans = _server->getServChannels();
	std::vector<std::string> channelNames;

	if (arguments.size() > 0)
		channelNames = ft_split(arguments[0], ',');

	for(unsigned long i=0; i < chans.size(); i++)
	{
		if (arguments.empty() || isInChannelsList(chans[i], channelNames))
			client->reply(RPL_LIST(client->getNickName(), chans[i]->getName(), intToString(chans[i]->getNbrClients()), "No topic is set"));
	}

	client->reply(RPL_LISTEND(client->getNickName()));
}
