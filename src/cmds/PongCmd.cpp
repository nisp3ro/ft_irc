#include "ft_irc.hpp"

/**
 * @brief Constructs a new PongCommand object.
 *
 * Initializes the PONG command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
PongCommand::PongCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the PongCommand object.
 *
 * Cleans up any resources used by the PongCommand object.
 */
PongCommand::~PongCommand() {}

/**
 * @brief Executes the PONG command.
 *
 * Processes a client's PONG command, which is typically used in response to a PING.
 * The function performs the following steps:
 * 1. Checks if the required parameter is provided; if not, it sends an ERR_NEEDMOREPARAMS reply.
 * 2. Sends a PING reply (using RPL_PING) back to the client, effectively acknowledging the PONG.
 *
 * @param client Pointer to the Client object issuing the PONG command.
 * @param arguments A vector of strings containing the command parameters.
 */
void PongCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.empty()) {
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "PONG"));
		return;
	}

	client->write(RPL_PING(client->getPrefix(), arguments.at(0)));
}
