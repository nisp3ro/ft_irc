#include "ft_irc.hpp"

/**
 * @brief Constructs a new PingCommand object.
 *
 * Initializes the PING command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
PingCommand::PingCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the PingCommand object.
 *
 * Cleans up any resources used by the PingCommand object.
 */
PingCommand::~PingCommand() {}

/**
 * @brief Executes the PING command.
 *
 * Processes a client's PING command to check the connection.
 * The function performs the following steps:
 * 1. Validates that a parameter is provided; if not, it replies with ERR_NEEDMOREPARAMS.
 * 2. Sends a PING reply (RPL_PING) back to the client using the provided parameter.
 *
 * @param client Pointer to the Client object issuing the PING command.
 * @param arguments A vector of strings containing the command parameters.
 */
void PingCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.empty()) {
		client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "PING"));
		return;
	}

	client->write(RPL_PING(client->getPrefix(), arguments.at(0)));
}
