#include "ft_irc.hpp"

/**
 * @brief Constructs a new QuitCommand object.
 *
 * Initializes the QUIT command handler by invoking the base Command constructor.
 * The 'authRequired' parameter indicates whether authentication is required to execute this command.
 *
 * @param server Pointer to the Server instance.
 * @param authRequired Boolean flag indicating if authentication is required.
 */
QuitCommand::QuitCommand(Server *server, bool authRequired) : Command(server, authRequired) {}

/**
 * @brief Destroys the QuitCommand object.
 *
 * Cleans up any resources used by the QuitCommand object.
 */
QuitCommand::~QuitCommand() {}

/**
 * @brief Executes the QUIT command.
 *
 * Processes a client's QUIT command, which is used to disconnect from the server.
 * The function performs the following steps:
 * 1. Determines a quit reason. If no argument is provided, a default reason "Leaving..." is used.
 * 2. If the provided reason begins with a colon (':'), the colon is removed.
 * 3. Sends a quit reply (RPL_QUIT) to the client with the client's prefix and quit reason.
 * 4. Deletes the client from the server by removing its file descriptor.
 *
 * @param client Pointer to the Client object issuing the QUIT command.
 * @param arguments A vector of strings containing the command parameters.
 */
void QuitCommand::execute(Client *client, std::vector<std::string> arguments) {

	std::string reason = arguments.empty() ? "Leaving..." : arguments.at(0);
	reason = reason.at(0) == ':' ? reason.substr(1) : reason;

	client->write(RPL_QUIT(client->getPrefix(), reason));
	this->_server->delClient(client->getFD());
}
