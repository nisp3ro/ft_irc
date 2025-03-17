#include "ft_irc.hpp"

/**
 * @brief Constructs a new PartCommand object.
 *
 * Initializes the PART command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
PartCommand::PartCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the PartCommand object.
 *
 * Cleans up any resources used by the PartCommand object.
 */
PartCommand::~PartCommand() {}

/**
 * @brief Executes the PART command.
 *
 * Processes a client's request to leave a channel. The expected format is:
 * "PART <channel>{,<channel>} [<reason>]". The function performs the following steps:
 *
 * 1. Checks if the required channel parameter is provided; if not, sends an ERR_NEEDMOREPARAMS error.
 * 2. Extracts the channel name from the first argument.
 * 3. Assembles a reason for parting from any additional arguments.
 *    - If a reason is provided and starts with a colon (':'), the colon is removed.
 *    - Any trailing space is also removed.
 * 4. Retrieves the channel by its name from the server. If the channel does not exist,
 *    an ERR_NOSUCHCHANNEL error is sent to the client.
 * 5. Checks if the client is actually a member of the channel by iterating through the client's
 *    list of joined channels. If not, an ERR_NOTONCHANNEL error is sent.
 * 6. If all validations pass, the client's leave() method is called with the channel and reason.
 *
 * @param client Pointer to the Client object issuing the PART command.
 * @param arguments A vector of strings containing the command parameters.
 */
void PartCommand::execute(Client *client, std::vector<std::string> arguments) {

	if (arguments.empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "PART"));
		return;
	}

	std::string name = arguments[0];
	std::string reason = "";

	// Assemble reason from additional arguments, if any.
	if (arguments.size() >= 2)
		for (std::vector<std::string>::iterator it = arguments.begin() + 1; it != arguments.end(); it++)
			reason.append(*it + " ");
	// Remove the leading colon if present.
	if (reason[0] == ':')
		reason = reason.substr(1, reason.size());
	// Remove the trailing space if present.
	if (reason[reason.size() - 1] == ' ')
		reason = reason.substr(0, reason.size() - 1);

	// Retrieve the channel by name.
	Channel *channel = _server->getChannel(name);
	if (!channel)
	{
		client->reply(ERR_NOSUCHCHANNEL(client->getNickName(), name));
		return;
	}

	// Check if the client is actually in the channel.
	std::vector<Channel *> chans = client->getUserChans();
	std::vector<Channel *>::iterator it = chans.begin();
	Channel *chan;
	while (it != chans.end())
	{
		chan = it.operator*();
		if (chan->getName() == name)
			break;
		++it;
	}
	// If the client is not in the channel, send an error reply.
	if (it == chans.end())
	{
		client->write(ERR_NOTONCHANNEL(client->getNickName(), name));
		return;
	}

	// Process the client's departure from the channel.
	client->leave(chan, 0, reason);
}
