#include "ft_irc.hpp"

/**
 * @brief Constructs a new ListCommand object.
 *
 * Initializes the LIST command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
ListCommand::ListCommand(Server *server) : Command(server) {};

/**
 * @brief Destroys the ListCommand object.
 *
 * Cleans up any resources used by the ListCommand object.
 */
ListCommand::~ListCommand() {};

/**
 * @brief Checks if a channel's name is present in the provided list of channel names.
 *
 * Iterates through the vector of channel name strings and compares each with the name of the given channel.
 *
 * @param channel Pointer to the Channel object to check.
 * @param channelNames A vector of strings representing channel names.
 * @return true if the channel's name is found in the list, false otherwise.
 */
bool isInChannelsList(Channel *channel, std::vector<std::string> channelNames)
{
	for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		if (channel->getName() == *it)
			return true;
	}
	return false;
}

/**
 * @brief Executes the LIST command.
 *
 * Processes a client's LIST command by retrieving a list of channels from the server. If arguments are provided,
 * the first argument is expected to be a comma-separated list of channel names to filter by. The function iterates
 * over all server channels and, for each channel that matches the filter (or for all channels if no filter is provided),
 * sends a reply to the client containing the channel's name, the number of clients in the channel, and a placeholder topic.
 * Finally, it sends an end-of-list reply.
 *
 * @param client Pointer to the Client object issuing the LIST command.
 * @param arguments A vector of strings containing command arguments; if non-empty, the first element should be a comma-separated list of channel names.
 */
void ListCommand::execute(Client *client, std::vector<std::string> arguments)
{
	std::vector<Channel *> chans = _server->getServChannels();
	std::vector<std::string> channelNames;

	// If arguments are provided, split the first argument by commas to create a filter list.
	if (arguments.size() > 0)
		channelNames = ft_split(arguments[0], ',');

	// Iterate over all channels on the server.
	for (unsigned long i = 0; i < chans.size(); i++)
	{
		// If no filter is provided or the channel's name is in the filter list, send the channel's info to the client.
		if (arguments.empty() || isInChannelsList(chans[i], channelNames))
			client->reply(RPL_LIST(client->getNickName(), chans[i]->getName(), intToString(chans[i]->getNbrClients()), "No topic is set"));
	}

	// Send end-of-list reply to the client.
	client->reply(RPL_LISTEND(client->getNickName()));
}
