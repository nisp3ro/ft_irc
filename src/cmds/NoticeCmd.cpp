#include "ft_irc.hpp"

/**
 * @brief Constructs a new NoticeCommand object.
 *
 * Initializes the NOTICE command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
NoticeCommand::NoticeCommand(Server *server) : Command(server) {};

/**
 * @brief Destroys the NoticeCommand object.
 *
 * Cleans up any resources used by the NoticeCommand object.
 */
NoticeCommand::~NoticeCommand() {};

/**
 * @brief Executes the NOTICE command.
 *
 * Processes a NOTICE command issued by a client. The NOTICE command sends a message
 * without expecting an automatic reply, either to another client or to a channel.
 *
 * The function performs the following steps:
 * 1. Validates that at least two arguments are provided and that neither the target nor
 *    the message is empty. If validation fails, the function returns without sending an error.
 * 2. Extracts the target (client nickname or channel name) from the first argument.
 * 3. Assembles the message from the remaining arguments. If the message begins with a colon (':'),
 *    the colon is removed.
 * 4. If the target starts with a '#' (indicating a channel), the function checks whether the issuing
 *    client is a member of that channel. If not, it returns without sending an error.
 * 5. For channel targets, the message is broadcast to the channel using the channel's broadcast method,
 *    excluding the sending client.
 * 6. If the target is not a channel, the function retrieves the destination client by nickname.
 *    If the destination client is found, the message is sent directly to that client.
 *
 * @param client Pointer to the Client object issuing the NOTICE command.
 * @param arguments A vector of strings containing the command parameters. The first element should be
 *                  the target, and the remaining elements form the message.
 */
void NoticeCommand::execute(Client *client, std::vector<std::string> arguments) {

	if (arguments.size() < 2 || arguments[0].empty() || arguments[1].empty()) {
		// Not enough parameters provided; NOTICE does not send an error reply.
		return;
	}

	std::string target = arguments.at(0);
	std::string message;

	// Assemble the message from the remaining arguments.
	for (std::vector<std::string>::iterator it = arguments.begin() + 1; it != arguments.end(); it++) {
		message.append(*it + " ");
	}

	// If the message starts with a colon, remove it.
	message = message.at(0) == ':' ? message.substr(1) : message;

	// If the target is a channel (starts with '#'):
	if (target.at(0) == '#')
	{
		// Retrieve the list of channels the client is part of.
		std::vector<Channel *> client_chans = client->getUserChans();
		std::vector<Channel *>::iterator it = client_chans.begin();

		Channel *chan;
		// Look for the channel with the matching name.
		while (it != client_chans.end())
		{
			chan = it.operator*();
			if (chan->getName() == target)
				break;
			++it;
		}
		// If the client is not on the target channel, do nothing.
		if (it == client_chans.end())
		{
			// The client is not on this channel; NOTICE does not send an error reply.
			return;
		}

		// Broadcast the notice to all channel members, excluding the sender.
		chan->broadcast(RPL_NOTICE(client->getPrefix(), target, message), client);
		return;
	}

	// If the target is a user, retrieve the destination client.
	Client *dest = _server->getClient(target);
	if (!dest)
	{
		// If no such client exists, do nothing.
		return;
	}
	// Send the notice directly to the destination client.
	dest->write(RPL_NOTICE(client->getPrefix(), target, message));
}
