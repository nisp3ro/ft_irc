#include "ft_irc.hpp"

/**
 * @brief Constructs a new CommandHandler object.
 *
 * Initializes the command handler for the server by mapping command strings (e.g., "PASS", "NICK", etc.)
 * to their corresponding Command objects. These commands are used to process incoming client messages.
 *
 * @param server Pointer to the Server instance to which this command handler belongs.
 */
CommandHandler::CommandHandler(Server *server) : _server(server)
{
	_commands["PASS"] = new PassCommand(_server, false);
	_commands["NICK"] = new NickCommand(_server, false);
	_commands["USER"] = new UserCommand(_server, false);
	_commands["QUIT"] = new QuitCommand(_server, false);
	_commands["PING"] = new PingCommand(_server);
	_commands["PONG"] = new PongCommand(_server);
	_commands["JOIN"] = new JoinCommand(_server);
	_commands["MODE"] = new ModeCommand(_server);
	_commands["PART"] = new PartCommand(_server);
	_commands["KICK"] = new KickCommand(_server);
	_commands["INVITE"] = new InvitCommand(_server);
	_commands["PRIVMSG"] = new PrivMsgCommand(_server);
	_commands["NOTICE"] = new NoticeCommand(_server);
	_commands["WHO"] = new WhoCommand(_server);
	_commands["LIST"] = new ListCommand(_server);
}

/**
 * @brief Destroys the CommandHandler object.
 *
 * Cleans up all dynamically allocated Command objects stored in the _commands map,
 * and clears the map.
 */
CommandHandler::~CommandHandler()
{
	for (std::map<std::string, Command *>::iterator it = _commands.begin(); it != _commands.end(); it++)
		delete it->second;
	_commands.clear();
}

/**
 * @brief Invokes the appropriate command based on the client's message.
 *
 * Parses the message received from the client, extracts the command name and its arguments,
 * checks if the command requires authentication, and then executes the command.
 * If the command is not recognized (and not the "CAP" command), an error reply is sent to the client.
 *
 * @param client Pointer to the Client object that sent the message.
 * @param message The raw message string received from the client.
 */
void CommandHandler::invoke(Client *client, const std::string &message)
{
	std::stringstream ssMessage(message);
	std::string syntax;

	while (std::getline(ssMessage, syntax))
	{
		// Remove the carriage return character if present at the end of the syntax string.
		syntax = syntax.substr(0, syntax[syntax.length() - 1] == '\r' ? syntax.length() - 1 : syntax.length());
		// Extract the command name from the syntax.
		std::string name = syntax.substr(0, syntax.find(' '));

		try
		{
			// Retrieve the command from the command map.
			Command *command = _commands.at(name);

			std::vector<std::string> arguments;
			std::string buf;
			std::stringstream ss(syntax.substr(name.length(), syntax.length()));

			// Extract command arguments into a vector.
			while (ss >> buf)
			{
				arguments.push_back(buf);
			}

			// Check if the command requires authentication and if the client is registered.
			if (command->authRequired() && !client->isRegistered())
			{
				client->reply(ERR_NOTREGISTERED(client->getNickName()));
				return;
			}

			// Execute the command with the client and the arguments.
			command->execute(client, arguments);
		}
		catch (const std::out_of_range &e)
		{
			// If the command is not recognized (and not the "CAP" command), send an unknown command error.
			if (name != "CAP")
				client->reply(ERR_UNKNOWNCOMMAND(client->getNickName(), name));
		}
	}
}
