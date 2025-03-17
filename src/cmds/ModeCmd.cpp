#include "ft_irc.hpp"

/**
 * @brief Constructs a new ModeCommand object.
 *
 * Initializes the MODE command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
ModeCommand::ModeCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the ModeCommand object.
 *
 * Cleans up any resources used by the ModeCommand object.
 */
ModeCommand::~ModeCommand() {}

/**
 * @brief Executes the MODE command.
 *
 * This function processes the MODE command issued by a client to change channel settings.
 * It expects at least two arguments:
 *  - The target channel name.
 *  - The mode string indicating which modes to set or unset.
 *
 * The function performs the following steps:
 * 1. Checks that at least two arguments are provided and that they are not empty.
 * 2. Retrieves the channel specified by the target argument. If the channel does not exist,
 *    an error (ERR_NOSUCHCHANNEL) is sent back to the client.
 * 3. Verifies that the client issuing the command is the channel admin or an operator. If not,
 *    an error (ERR_CHANOPRIVSNEEDED) is sent.
 * 4. Iterates over each character in the mode string (second argument). For each mode character:
 *    - 'i': Toggles the invite-only status of the channel.
 *    - 'l': Sets or unsets the maximum number of clients allowed in the channel.
 *    - 'k': Sets or removes the channel password.
 *    - 'o': Adds or removes a channel operator. For this mode, an additional parameter is expected
 *           specifying the nickname of the client to add or remove as an operator.
 * 5. For each mode change, the function broadcasts a mode change reply (RPL_MODE) to all channel members.
 *
 * @param client Pointer to the Client object issuing the MODE command.
 * @param arguments A vector of strings containing the command parameters.
 */
void ModeCommand::execute(Client *client, std::vector<std::string> arguments)
{
	if (arguments.size() < 2 || arguments[0].empty() || arguments[1].empty()) {
		return;
	}

	std::string target = arguments.at(0);

	Channel *channel = _server->getChannel(target);
	if (!channel)
	{
		client->reply(ERR_NOSUCHCHANNEL(client->getNickName(), target));
		return;
	}

	// Check if the client is the channel admin or an operator.
	if (channel->getAdmin() != client && !channel->is_oper(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickName(), target));
		return;
	}

	int i = 0;
	int p = 2;
	char c;

	while ((c = arguments[1][i])) {

		char prevC = i > 0 ? arguments[1][i - 1] : '\0';
		bool active = prevC == '+';

		switch (c) {

			case 'i': {
				// Toggle invite-only mode.
				channel->setInviteOnly(active);
				channel->broadcast(RPL_MODE(client->getPrefix(), channel->getName(), (active ? "+i" : "-i"), ""));
				break;
			}

			case 'l': {
				// Set or remove the limit on the maximum number of clients.
				channel->setMaxClients(active ? std::atoi(arguments[p].c_str()) : 0);
				channel->broadcast(RPL_MODE(client->getPrefix(), channel->getName(), (active ? "+l" : "-l"), (active ? arguments[p] : "")));
				p += active ? 1 : 0;
				break;
			}

			case 'k': {
				// Set or remove the channel password.
				channel->setPassword(active ? arguments[p] : "");
				channel->broadcast(RPL_MODE(client->getPrefix(), channel->getName(), (active ? "+k" : "-k"), (active ? arguments[p] : "")));
				p += active ? 1 : 0;
				break;
			}

			case 'o': {
				// Add or remove a channel operator.
				Client *c_tar = channel->getClient(arguments[p]);
				if (!c_tar)
				{
					channel->broadcast(ERR_USERNOTINCHANNEL(client->getNickName(), arguments[p], channel->getName()));
					return ;
				}
				if (active)
					channel->addOper(c_tar);
				else
					channel->removeOper(c_tar);
				channel->broadcast(RPL_MODE(client->getPrefix(), channel->getName(), (active ? "+o" : "-o"), (c_tar->getNickName())));
				p += active ? 1 : 0;
				break;
			}

			default:
				break;
		}
		i++;
	}
}
