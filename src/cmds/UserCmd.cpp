#include "ft_irc.hpp"

/**
 * @brief Constructs a new UserCommand object.
 *
 * Initializes the USER command handler by invoking the base Command constructor.
 * The 'auth' parameter indicates whether authentication is required for this command.
 *
 * @param server Pointer to the Server instance.
 * @param auth Boolean flag indicating if authentication is required.
 */
UserCommand::UserCommand(Server *server, bool auth) : Command(server, auth) {}

/**
 * @brief Destroys the UserCommand object.
 *
 * Cleans up any resources used by the UserCommand object.
 */
UserCommand::~UserCommand() {}

/**
 * @brief Executes the USER command.
 *
 * Processes a client's USER command, which is used to provide the username and real name
 * during registration. The function performs the following steps:
 *
 * 1. Checks if the client is already registered. If so, it sends an ERR_ALREADYREGISTERED reply.
 * 2. Validates that at least four parameters are provided; if not, it sends an ERR_NEEDMOREPARAMS reply.
 * 3. Sets the client's username using the first parameter.
 * 4. Sets the client's real name using the fourth parameter (after removing the leading colon).
 * 5. Calls the welcome() method on the client, which sends welcome messages if the client is fully registered.
 *
 * @param client Pointer to the Client object issuing the USER command.
 * @param arguments A vector of strings containing the command parameters.
 */
void UserCommand::execute(Client *client, std::vector<std::string> arguments) {

	if (client->isRegistered())
	{
		client->reply(ERR_ALREADYREGISTERED(client->getPrefix()));
		return;
	}

	if (arguments.size() < 4) {
		client->reply(ERR_NEEDMOREPARAMS(client->getPrefix(), "USER"));
		return;
	}

	client->setUsername(arguments[0]);
	client->setRealName(arguments[3].substr(1));
	client->welcome();
}
