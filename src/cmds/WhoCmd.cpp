#include "ft_irc.hpp"

/**
 * @brief Constructs a new WhoCommand object.
 *
 * Initializes the WHO command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
WhoCommand::WhoCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the WhoCommand object.
 *
 * Cleans up any resources used by the WhoCommand object.
 */
WhoCommand::~WhoCommand() {}

/**
 * @brief Executes the WHO command.
 *
 * Processes a client's WHO command, which is used to list information about users.
 * The behavior depends on the number of arguments provided:
 *
 * - If no arguments are provided, the command replies with a WHO reply for every client
 *   connected to the server, using "*" as the channel name.
 *
 * - If one argument is provided and it begins with '#', the command treats it as a channel name.
 *   It retrieves the channel and replies with a WHO reply for every client in that channel.
 *
 * After sending the individual WHO replies, an end-of-who reply (RPL_ENDOFWHO) is sent.
 *
 * @param client Pointer to the Client object issuing the WHO command.
 * @param arguments A vector of strings containing the command parameters.
 */
void WhoCommand::execute(Client *client, std::vector<std::string> arguments)
{
	std::string channelName = "*";

	if (arguments.empty())
	{
		// No arguments: list all clients on the server.
		std::vector<Client *> clients = _server->getServClients();
		for (unsigned long i = 0; i < clients.size(); i++)
			client->reply(RPL_WHOREPLY(client->getNickName(),
			                            channelName,
			                            clients[i]->getUserName(),
			                            clients[i]->getHostName(),
			                            this->_server->getServerName(),
			                            clients[i]->getNickName(),
			                            clients[i]->getRealName()));
	}
	else if (arguments.size() == 1)
	{
		// One argument provided: check if it's a channel (starts with '#').
		if (arguments[0].at(0) == '#')
		{
			channelName = arguments[0];
			Channel *channel = _server->getChannel(channelName);
			if (channel)
			{
				// List only the clients in the specified channel.
				std::vector<Client *> clients = channel->getChanClients();
				for (unsigned long i = 0; i < clients.size(); i++)
					client->reply(RPL_WHOREPLY(client->getNickName(),
					                            channelName,
					                            clients[i]->getUserName(),
					                            clients[i]->getHostName(),
					                            this->_server->getServerName(),
					                            clients[i]->getNickName(),
					                            clients[i]->getRealName()));
			}
		}
	}
	// Send end-of-WHO reply.
	client->reply(RPL_ENDOFWHO(client->getNickName(), channelName));
	return;
}
