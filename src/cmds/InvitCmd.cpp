#include "ft_irc.hpp"

InvitCommand::InvitCommand(Server *server) : Command(server) {}

InvitCommand::~InvitCommand() {}

void InvitCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.size() < 2)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "INVITE"));
		return;
	}
	std::string target = arguments[0];
	std::string chan_name = arguments[1];

	// Check if inviter is in channel
	Channel *chan = this->_server->getChannel(chan_name);
	if (!chan || !chan->isInChannel(client))
	{
		client->reply(ERR_NOTONCHANNEL(client->getNickName(), chan_name));
		return;
	}

	// If channel is invit only, check that inviter is admin or operator
	if (chan->invitOnlyChan() && chan->getAdmin() != client && !chan->is_oper(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickName(), chan->getName()));
		return;
	}

	// Check that the target nickname is valid
	Client *user = _server->getClient(target);
	if (!user)
	{
		client->reply(ERR_NOSUCHNICK(client->getNickName(), target));
		return ;
	}

	// Check if target is already in chan
	if (chan->isInChannel(user))
	{
		client->reply(ERR_USERONCHANNEL(client->getNickName(), user->getNickName(), chan_name));
		return;
	}

	chan->invit(client, user);
}
