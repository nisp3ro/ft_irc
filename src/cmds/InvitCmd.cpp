#include "ft_irc.hpp"

/**
 * @brief Constructs a new InvitCommand object.
 *
 * Initializes the INVITE command handler for the server by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
InvitCommand::InvitCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the InvitCommand object.
 *
 * Cleans up any resources used by the InvitCommand object.
 */
InvitCommand::~InvitCommand() {}

/**
 * @brief Executes the INVITE command.
 *
 * This function processes the INVITE command from a client. It expects at least two parameters:
 * 1. The target nickname to invite.
 * 2. The channel name into which the target is being invited.
 *
 * The execution flow is as follows:
 * - If there are fewer than two arguments, an error is sent back to the client indicating more parameters are needed.
 * - The function retrieves the channel using the provided channel name. If the channel does not exist or the
 *   inviter is not in the channel, an error is returned.
 * - For invite-only channels, it verifies that the inviter is either the channel admin or an operator. If not, an error is sent.
 * - The target client is then retrieved using the target nickname. If the target does not exist, an error is returned.
 * - If the target is already present in the channel, an error is sent indicating the user is already in the channel.
 * - Otherwise, the target is invited to the channel using the channel's invit() method.
 *
 * @param client Pointer to the Client object issuing the INVITE command.
 * @param arguments A vector of strings containing the command parameters (target nickname and channel name).
 */
void InvitCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.size() < 2)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "INVITE"));
		return;
	}
	std::string target = arguments[0];
	std::string chan_name = arguments[1];

	// Check if the inviter is in the specified channel.
	Channel *chan = this->_server->getChannel(chan_name);
	if (!chan || !chan->isInChannel(client))
	{
		client->reply(ERR_NOTONCHANNEL(client->getNickName(), chan_name));
		return;
	}

	// For invite-only channels, check that the inviter is either the admin or an operator.
	if (chan->invitOnlyChan() && chan->getAdmin() != client && !chan->is_oper(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickName(), chan->getName()));
		return;
	}

	// Check that the target nickname is valid and exists.
	Client *user = _server->getClient(target);
	if (!user)
	{
		client->reply(ERR_NOSUCHNICK(client->getNickName(), target));
		return ;
	}

	// Check if the target is already in the channel.
	if (chan->isInChannel(user))
	{
		client->reply(ERR_USERONCHANNEL(client->getNickName(), user->getNickName(), chan_name));
		return;
	}

	// Invite the target to the channel.
	chan->invit(client, user);
}
