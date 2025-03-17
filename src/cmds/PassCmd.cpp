#include "ft_irc.hpp"

/**
 * @brief Constructs a new PassCommand object.
 *
 * Initializes the PASS command handler by invoking the base Command constructor.
 * The 'auth' parameter indicates whether the command requires authentication.
 *
 * @param server Pointer to the Server instance.
 * @param auth Boolean flag indicating if authentication is required.
 */
PassCommand::PassCommand(Server *server, bool auth) : Command(server, auth) {}

/**
 * @brief Destroys the PassCommand object.
 *
 * Cleans up any resources used by the PassCommand object.
 */
PassCommand::~PassCommand() {}

/**
 * @brief Executes the PASS command.
 *
 * Processes a client's PASS command, which is used to provide the server password.
 * The function performs the following steps:
 *
 * 1. If the client is already registered, it sends an ERR_ALREADYREGISTERED reply.
 * 2. Checks if a password parameter is provided. If not, it sends an ERR_NEEDMOREPARAMS reply.
 * 3. Compares the provided password (after removing a leading colon, if present) with the server's password.
 *    If the passwords do not match, it sends an ERR_PASSWDMISMATCH reply.
 * 4. If the password is correct, it marks the client as having entered the correct password and
 *    calls the welcome() method to proceed with registration.
 *
 * @param client Pointer to the Client object issuing the PASS command.
 * @param arguments A vector of strings containing the command parameters. The first parameter should be the password.
 */
void PassCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (client->isRegistered())
	{
		client->reply(ERR_ALREADYREGISTERED(client->getPrefix()));
		return;
	}

	if (arguments.empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getPrefix(), "PASS"));
		return;
	}

	// Remove a leading colon if present in the password argument and compare it with the server password.
	if (_server->getPassword() != arguments[0].substr(arguments[0][0] == ':' ? 1 : 0))
	{
		client->reply(ERR_PASSWDMISMATCH(client->getPrefix()));
		return;
	}

	// Mark the client as having provided the correct password and send welcome messages.
	client->setCorrectPassword(true);
	client->welcome();
}
