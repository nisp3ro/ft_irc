#include "ft_irc.hpp"
#include "Replies.hpp"

/**
 * @brief Channel constructor.
 *
 * Initializes a new Channel instance with the specified name, password, admin client, and server.
 * The channel is configured with a default limit (1000) and flags.
 *
 * @param name The name of the channel.
 * @param password The password of the channel.
 * @param admin Pointer to the Client who will be the admin of the channel.
 * @param server Pointer to the Server instance managing this channel.
 */
// Channel::Channel(std::string const &name, std::string const &password, Client *admin, Server *server)
// 					: _name(name) , _admin(admin), _l(1000), _i(false), _k(password), _server(server) {}
Channel::Channel(std::string const &name, std::string const &password, Client *admin, Server *server)
 					: _name(name), _admin(admin), _l(1000), _i(false), _k(password), _topic(""),
					_topicRestricted(false), _server(server) { }


/**
 * @brief Channel destructor.
 *
 * Cleans up any resources used by the Channel instance.
 */
Channel::~Channel() {}

/**
 * @brief Retrieves the nicknames of all clients in the channel.
 *
 * Iterates through the list of clients in the channel and constructs a vector of their nicknames.
 * The admin client's nickname is prefixed with "@".
 *
 * @return std::vector<std::string> A vector containing the nicknames of the clients in the channel.
 */
std::vector<std::string> Channel::getNickNames()
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

/**
 * @brief Broadcasts a message to all clients in the channel.
 *
 * Delegates the broadcast operation to the server's broadcastChannel() function.
 *
 * @param message The message to be broadcast.
 */
void Channel::broadcast(const std::string &message)
{
	// std::vector<Client *>::iterator it;;
	// for (it = _clients.begin(); it != _clients.end(); it++)
	// 	(*it)->write(message);
	this->_server->broadcastChannel(message, this);
}

/**
 * @brief Broadcasts a message to all clients in the channel except the specified client.
 *
 * Delegates the broadcast operation to the server's broadcastChannel() function,
 * excluding the client whose file descriptor matches the one provided.
 *
 * @param message The message to be broadcast.
 * @param exclude Pointer to the client to exclude from the broadcast.
 */
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

/**
 * @brief Retrieves a client from the channel by nickname.
 *
 * Searches through the channel's client list for a client whose nickname matches the provided string.
 *
 * @param nickname The nickname of the client to find.
 * @return Client* Pointer to the client with the specified nickname, or NULL if not found.
 */
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

/**
 * @brief Checks if a client is an operator in the channel.
 *
 * Iterates through the list of operator clients and determines whether the provided client is in that list.
 *
 * @param client Pointer to the client to check.
 * @return int Returns 1 if the client is an operator, or 0 otherwise.
 */
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

/**
 * @brief Removes a client from the channel.
 *
 * Broadcasts a PART message to all channel members indicating the client's departure,
 * removes the client from both the operator list and the general client list, and calls the client's leave() method.
 * If the departing client is the admin and there are other clients remaining, the admin is reassigned.
 *
 * @param client Pointer to the client to remove.
 * @param reason A string containing the reason for the client leaving.
 */
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

/**
 * @brief Removes an operator from the channel.
 *
 * Removes the specified client from the list of channel operators.
 *
 * @param client Pointer to the client to remove from operator status.
 */
void Channel::removeOper(Client *client)
{
	_oper_clients.erase(this->_oper_clients.begin() + this->_clientIndex(_oper_clients, client));
}

/**
 * @brief Kicks a client from the channel.
 *
 * Broadcasts a KICK message to all channel members, then removes the target client from the channel.
 *
 * @param client Pointer to the client performing the kick.
 * @param target Pointer to the client being kicked.
 * @param reason A string containing the reason for kicking the client.
 */
void Channel::kick(Client *client, Client *target, std::string reason)
{
	broadcast(RPL_KICK(client->getPrefix(), _name, target->getNickName(), reason));
	reason.clear();
	removeClient(target, reason);
}

/**
 * @brief Invites a client to the channel.
 *
 * Sends an invitation message to the target client, and automatically joins the target to the channel.
 *
 * @param client Pointer to the client sending the invitation.
 * @param target Pointer to the client being invited.
 */
void Channel::invit(Client *client, Client *target)
{
	client->reply(RPL_INVITING(client->getNickName(), target->getNickName(), this->_name));
	target->write(RPL_INVITE(client->getPrefix(), target->getNickName(), this->_name));
	target->join(this);
}

/**
 * @brief Checks if a client is a member of the channel.
 *
 * Iterates through the list of clients in the channel and returns true if the client is found.
 *
 * @param client Pointer to the client to check.
 * @return bool True if the client is in the channel, false otherwise.
 */
bool Channel::isInChannel(Client *client)
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

/**
 * @brief Retrieves the index of a client in a given client list.
 *
 * Iterates through the provided vector of clients and returns the index of the specified client.
 * If the client is not found, returns 0.
 *
 * @param clients A vector of Client pointers in which to search.
 * @param client Pointer to the client to find.
 * @return unsigned long The index of the client in the vector, or 0 if not found.
 */
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
