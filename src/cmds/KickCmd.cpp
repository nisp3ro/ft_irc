#include "ft_irc.hpp"

/**
 * @brief Constructs a new KickCommand object.
 *
 * Initializes the KICK command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
KickCommand::KickCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the KickCommand object.
 *
 * Cleans up any resources used by the KickCommand object.
 */
KickCommand::~KickCommand() {}

/**
 * @brief Executes the KICK command.
 *
 * This function processes the KICK command issued by a client. The expected format is:
 * "KICK <channel> <user> *( "," <user> ) [<comment>]"
 *
 * The function performs the following steps:
 * 1. Validates that at least two arguments (channel and user) are provided.
 *    If not, it sends an ERR_NEEDMOREPARAMS reply to the client.
 * 2. Extracts the channel name and target user's nickname from the arguments.
 * 3. Assembles a reason for the kick from any additional arguments.
 *    - If no comment is provided, a default message "No reason specified." is used.
 *    - If the comment starts with a colon (":"), the colon is removed.
 *    - Any trailing spaces in the reason are removed.
 * 4. Checks if the client issuing the command (the kicker) is in the specified channel.
 *    If not, an ERR_NOTONCHANNEL reply is sent.
 * 5. Verifies that the kicker has sufficient privileges (i.e., is the channel admin or an operator).
 *    If not, an ERR_CHANOPRIVSNEEDED reply is sent.
 * 6. Retrieves the target client using the provided nickname and verifies that the target is present in the channel.
 *    If the target is not in the channel, an ERR_USERNOTINCHANNEL reply is sent.
 * 7. If all checks pass, the target is kicked from the channel with the provided reason.
 *
 * @param client Pointer to the Client object issuing the KICK command.
 * @param arguments A vector of strings containing the command parameters.
 */
void KickCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.size() < 2)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "KICK"));
		return;
	}
	std::string chan_name = arguments[0];
	std::string target = arguments[1];
	std::string reason = "No reason specified.";

	// Assemble reason from additional arguments if provided.
	if (arguments.size() >= 3)
	{
		reason = "";
		for (std::vector<std::string>::iterator it = arguments.begin() + 2; it != arguments.end(); it++)
			reason.append(*it + " ");
	}
	// Remove the leading colon if present in the reason.
	if (reason[0] == ':')
		reason = reason.substr(1, reason.size());
	// Remove the trailing space if present.
	if (reason[reason.size() - 1] == ' ')
		reason = reason.substr(0, reason.size() - 1);

	// Check if the kicker (client) is in the specified channel.
	Channel *chan = this->_server->getChannel(chan_name);
	if (!chan || !chan->isInChannel(client))
	{
		client->reply(ERR_NOTONCHANNEL(client->getNickName(), chan_name));
		return;
	}

	// Check if the kicker has operator privileges or is the channel admin.
	if (chan->getAdmin() != client && !chan->is_oper(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickName(), chan->getName()));
		return;
	}

	// Retrieve the target client and check if the target is in the channel.
	Client *user = this->_server->getClient(target);
	if (!chan || !chan->isInChannel(user))
	{
		client->reply(ERR_USERNOTINCHANNEL(client->getNickName(), user->getNickName(), chan_name));
		return;
	}

	// Kick the target client from the channel with the provided reason.
	chan->kick(client, user, reason);
}
