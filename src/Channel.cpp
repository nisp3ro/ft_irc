#include "ft_irc.hpp"
#include "Replies.hpp"

Channel::Channel(std::string const &name, std::string const &password, Client *admin, Server *server)
					: _name(name) , _admin(admin), _l(1000), _i(false), _k(password), _server(server) {}
Channel::~Channel() {}

std::vector<std::string>	Channel::getNickNames()
{
	std::vector<std::string> nicknames;
	std::vector<Client *>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		Client *client = it.operator*();
		nicknames.push_back((_admin == client ? "@" : "") + (*it)->getNickName());
		it++;
	}
	return nicknames;
}

void Channel::broadcast(const std::string &message)
{
	// std::vector<Client *>::iterator it;;
	// for (it = _clients.begin(); it != _clients.end(); it++)
	// 	(*it)->write(message);
	this->_server->broadcastChannel(message, this);
}

void Channel::broadcast(const std::string &message, Client *exclude)
{
	// std::vector<Client *>::iterator it;;
	// for (it = _clients.begin(); it != _clients.end(); it++)
	// {
	// 	if (*it == exclude)
	// 		continue;
	// 	(*it)->write(message);
	// }
	this->_server->broadcastChannel(message, exclude->getFD(), this);
}

Client *Channel::getClient(const std::string &nickname)
{
	std::vector<Client *>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		if ((*it)->getNickName() == nickname)
			return *it;
		it++;
	}
	return NULL;
}

int Channel::is_oper(Client *client)
{
	std::vector<Client *> opers_chan = this->getChanOpers();
	std::vector<Client *>:: iterator it_oper = opers_chan.begin();

	while (it_oper != opers_chan.end())
	{
		Client *oper = it_oper.operator*();
		if (oper == client)
			return 1;
		++it_oper;
	}
	if (it_oper == opers_chan.end())
		return 0;
	return 0;
}

void Channel::removeClient(Client *client, std::string reason)
{
	std::string clientPrefix = client->getPrefix();

	if (reason.empty())
		this->broadcast(RPL_PART(clientPrefix, this->getName()));
	else
		this->broadcast(RPL_PART_REASON(clientPrefix, this->getName(), reason));
	reason.clear();

	if (!_oper_clients.empty())
		_oper_clients.erase(this->_oper_clients.begin() + this->_clientIndex(_oper_clients, client));
	if (!_clients.empty())
		_clients.erase(this->_clients.begin() + this->_clientIndex(_clients, client));
	client->leave(this, 1, reason);

	if (_clients.empty())
	{
		// free chan and remove it from server
		return;
	}

	if (_admin == client)
		_admin = _clients.begin().operator*();

	// message to say that there is a new admin
}

void Channel::removeOper(Client *client)
{
	_oper_clients.erase(this->_oper_clients.begin() + this->_clientIndex(_oper_clients, client));
}

void Channel::kick(Client *client, Client *target, std::string reason)
{
	broadcast(RPL_KICK(client->getPrefix(), _name, target->getNickName(), reason));
	reason.clear();
	removeClient(target, reason);
}

void Channel::invit(Client *client, Client *target)
{
	client->reply(RPL_INVITING(client->getNickName(), target->getNickName(), this->_name));
	target->write(RPL_INVITE(client->getPrefix(), target->getNickName(), this->_name));
	target->join(this);
}

bool	Channel::isInChannel(Client *client)
{
	std::vector<Client *>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		if (*it == client)
			return true;
		it++;
	}
	return false;
}

unsigned long Channel::_clientIndex(std::vector<Client *> clients, Client *client)
{
	unsigned long i = 0;
	std::vector<Client *>::iterator it = clients.begin();

	while (it != clients.end())
	{
		if (*it == client)
			return i;
		it++;
		i++;
	}
	return 0;
}
