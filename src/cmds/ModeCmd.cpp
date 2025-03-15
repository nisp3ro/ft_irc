#include "ft_irc.hpp"

ModeCommand::ModeCommand(Server *server) : Command(server) {}

ModeCommand::~ModeCommand() {}

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

	// check if admin or chanop
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
				channel->setInviteOnly(active);
				channel->broadcast(RPL_MODE(client->getPrefix(), channel->getName(), (active ? "+i" : "-i"), ""));
				break;
			}

			case 'l': {
				channel->setMaxClients(active ? std::atoi(arguments[p].c_str()) : 0);
				channel->broadcast(RPL_MODE(client->getPrefix(), channel->getName(), (active ? "+l" : "-l"), (active ? arguments[p] : "")));
				p += active ? 1 : 0;
				break;
			}

			case 'k': {
				channel->setPassword(active ? arguments[p] : "");
				channel->broadcast(RPL_MODE(client->getPrefix(), channel->getName(), (active ? "+k" : "-k"), (active ? arguments[p] : "")));
				p += active ? 1 : 0;
				break;
			}

			case 'o': {
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
