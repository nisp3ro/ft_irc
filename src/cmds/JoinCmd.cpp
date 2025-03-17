#include "ft_irc.hpp"

/**
 * @brief Constructs a new JoinCommand object.
 *
 * Initializes the JOIN command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
JoinCommand::JoinCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the JoinCommand object.
 *
 * Cleans up any resources used by the JoinCommand object.
 */
JoinCommand::~JoinCommand() {}

/**
 * @brief Executes the JOIN command.
 *
 * This function processes a client's JOIN command. The expected format is:
 * "JOIN <channel>{,<channel>} [<key>{,<key>}]". It performs the following steps:
 * 
 * 1. Checks if the required parameters are provided. If not, sends an error (ERR_NEEDMOREPARAMS).
 * 2. Retrieves the channel name and optional password from the arguments.
 * 3. Looks up the channel in the server. If the channel does not exist, it is created.
 * 4. Checks if the channel is invite-only. If so, replies with an ERR_INVITEONLYCHAN error.
 * 5. Verifies if the client is already in the channel; if yes, it does nothing.
 * 6. Checks if the channel has reached its maximum number of users. If so, replies with ERR_CHANNELISFULL.
 * 7. Validates the provided password against the channel's password. If it does not match, sends ERR_BADCHANNELKEY.
 * 8. Finally, if all conditions are satisfied, the client is added to the channel using client->join(channel).
 *
 * @param client Pointer to the Client object issuing the JOIN command.
 * @param arguments A vector of strings containing the parameters for the JOIN command.
 */
void JoinCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "JOIN"));
		return;
	}

	std::string name = arguments[0];
	std::string password = arguments.size() > 1 ? arguments[1] : "";

	// Get the channel by name. Create it if it does not exist.
	Channel *channel = _server->getChannel(name);
	if (!channel)
		channel = _server->createChannel(name, password, client);

	// If the channel is invite-only, reject the join.
	if (channel->invitOnlyChan())
	{
		client->reply(ERR_INVITEONLYCHAN(client->getNickName(), channel->getName()));
		return;
	}

	// Check if the client is already in the channel.
	std::vector<Client *> clients = channel->getChanClients();
	std::vector<Client *>::iterator it;

	it = clients.begin();
	while (it != clients.end())
	{
		Client *cl = it.operator*();
		if (cl == client)
			return;
		it++;
	}

	// Check if the channel is full.
	if (channel->getMaxUsers() > 0 && channel->getNbrClients() >= channel->getMaxUsers())
	{
		client->reply(ERR_CHANNELISFULL(client->getNickName(), name));
		return;
	}

	// Check if the provided password matches the channel's password.
	if (channel->getPassword() != password)
	{
		client->reply(ERR_BADCHANNELKEY(client->getNickName(), name));
		return;
	}

	// Additional check for invite-only status (redundant if already checked above).
	if (channel->invitOnlyChan() == 1)
	{
		client->reply(ERR_INVITEONLYCHAN(client->getNickName(), name));
		return;
	}

	// Add the client to the channel.
	client->join(channel);
}
