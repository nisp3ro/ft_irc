#include "ft_irc.hpp"

/**
 * @brief Constructs a new PrivMsgCommand object.
 *
 * Initializes the PRIVMSG command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
PrivMsgCommand::PrivMsgCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the PrivMsgCommand object.
 *
 * Cleans up any resources used by the PrivMsgCommand object.
 */
PrivMsgCommand::~PrivMsgCommand() {}

/**
 * @brief Executes the PRIVMSG command.
 *
 * Processes a client's PRIVMSG command, which is used to send a private message to another client or to a channel.
 * The expected format is:
 * "PRIVMSG <target> <message>".
 *
 * The function performs the following steps:
 * 1. Validates that at least two arguments are provided and that neither the target nor the message is empty.
 *    If validation fails, it sends an ERR_NEEDMOREPARAMS reply.
 * 2. Extracts the target (client nickname or channel name) from the first argument.
 * 3. Assembles the message by concatenating the remaining arguments. If the message begins with a colon (':'),
 *    the colon is removed.
 * 4. If the target starts with '#' (indicating a channel), the function:
 *    - Retrieves the list of channels the client is a member of.
 *    - Searches for the specified channel in the client's list.
 *    - If the client is not in the channel, broadcasts an ERR_NOTONCHANNEL error to the server.
 *    - Otherwise, broadcasts the message to the channel (excluding the sender).
 * 5. If the target does not represent a channel:
 *    - Retrieves the destination client by nickname.
 *    - If the destination client is not found, sends an ERR_NOSUCHNICK reply.
 *    - Otherwise, sends the private message directly to the destination client.
 *
 * @param client Pointer to the Client object issuing the PRIVMSG command.
 * @param arguments A vector of strings containing the command parameters.
 */
void PrivMsgCommand::execute(Client *client, std::vector<std::string> arguments) {

	if (arguments.size() < 2 || arguments[0].empty() || arguments[1].empty()) {
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "PRIVMSG"));
		return;
	}

	std::string target = arguments.at(0);
	std::string message;

	// Assemble the message from the remaining arguments.
	for (std::vector<std::string>::iterator it = arguments.begin() + 1; it != arguments.end(); it++) {
		message.append(*it + " ");
	}

	// Remove leading colon from the message if present.
	message = message.at(0) == ':' ? message.substr(1) : message;

	// Check if the target is a channel (starts with '#').
	if (target.at(0) == '#') {

		std::vector<Channel *> client_chans = client->getUserChans();
		std::vector<Channel *>::iterator it = client_chans.begin();

		Channel *chan;
		// Search for the channel in which the client is a member.
		while (it != client_chans.end())
		{
			chan = it.operator*();
			if (chan->getName() == target)
				break;
			++it;
		}
		// If the client is not in the target channel, broadcast an error.
		if (it == client_chans.end())
		{
			_server->broadcast(ERR_NOTONCHANNEL(client->getNickName(), target));
			return;
		}

		// Broadcast the message to the channel, excluding the sender.
		chan->broadcast(RPL_PRIVMSG(client->getPrefix(), target, message), client);
		return;
	}

	// If the target is not a channel, retrieve the destination client.
	Client *dest = _server->getClient(target);
	if (!dest)
	{
		client->reply(ERR_NOSUCHNICK(client->getNickName(), target));
		return;
	}

	// Send the private message directly to the destination client.
	dest->write(RPL_PRIVMSG(client->getPrefix(), target, message));
}
