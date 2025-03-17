#include "ft_irc.hpp"

/**
 * @brief Constructs a new NickCommand object.
 *
 * Initializes the NICK command handler by invoking the base Command constructor.
 * The 'auth' parameter indicates whether the command requires authentication.
 *
 * @param server Pointer to the Server instance.
 * @param auth Boolean flag indicating if authentication is required.
 */
NickCommand::NickCommand(Server *server, bool auth) : Command(server, auth) {}

/**
 * @brief Destroys the NickCommand object.
 *
 * Cleans up any resources used by the NickCommand object.
 */
NickCommand::~NickCommand() {}

/**
 * @brief Executes the NICK command.
 *
 * Processes a client's request to change their nickname. The function performs the following steps:
 * 1. Checks if the required nickname parameter is provided; if not, sends an ERR_NONICKNAMEGIVEN error.
 * 2. Checks if the desired nickname is already in use by another client; if so, sends an ERR_NICKNAMEINUSE error.
 * 3. If the nickname is valid and available, sets the client's nickname to the provided value.
 * 4. Finally, it calls the welcome() function on the client, which sends the welcome messages if the client is fully registered.
 *
 * @param client Pointer to the Client object issuing the NICK command.
 * @param arguments A vector of strings containing the parameters for the NICK command.
 */
void NickCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.empty() || arguments[0].empty())
	{
		client->reply(ERR_NONICKNAMEGIVEN(client->getPrefix()));
		return;
	}

	std::string nickname = arguments[0];

	// Check if the nickname is already in use.
	if (_server->getClient(nickname))
	{
		client->reply(ERR_NICKNAMEINUSE(client->getPrefix(), nickname));
		return;
	}

	// Set the client's nickname and send welcome messages if appropriate.
	client->setNickname(nickname);
	client->welcome();
}
