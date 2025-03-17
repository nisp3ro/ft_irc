#include "ft_irc.hpp"
#include "Replies.hpp"

/**
 * @brief Constructs a Client instance representing a user connected to the server.
 * 
 *  Initializes the client with a server pointer, file descriptor, hostname, port,
 *  sets the correct password flag to false, and stores the server pointer.
 * 
 * @param server Pointer to the server instance managing the client.
 * @param fd File descriptor associated with the client's connection.
 * @param hostname The hostname of the client.
 * @param port The port number through which the client is connected.
 */
Client::Client(Server *server, int fd, std::string const &hostname, int port)
	: _fd(fd), _hostname(hostname), _port(port), _correct_password(false), _server(server) {}

/**
 * @brief Destructor for the Client class.
 */
Client::~Client() {}

/**
 * @brief  Sends a message to the client by calling the server's send function with the message
 * and the client's file descriptor.
 * 
 * @param message The message to be sent to the client.
 */
void Client::write(const std::string &message) const
{
	this->_server->send(message, this->getFD());
}

/**
 * @brief  Constructs and returns the client's prefix string.
 * If the nickname is empty, returns "*".
 * Otherwise, returns a string in the form "nickname!username@hostname",
 * omitting parts if username or hostname are empty.
 * 
 * @return std::string The constructed prefix.
 */
std::string Client::getPrefix() const
{
	if (this->getNickName().empty())
		return "*";
	return _nickname + (_username.empty() ? "" : "!" + _username) + (_hostname.empty() ? "" : "@" + _hostname);
}

/**
 * @brief Checks if the client is fully registered in the server.
 * 
 * A client is considered registered if they have a nickname, username, real name,
 * and have provided the correct password.
 * 
 * @return true If the client meets the registration requirements.
 * @return false Otherwise.
 */
bool Client::isRegistered() const
{
	return !this->getNickName().empty() && 
	       !this->getUserName().empty() && 
	       !this->getRealName().empty() && 
	       this->_correct_password;
}

/**
 * @brief Sends a server-formatted reply to the client.
 * 
 * @param reply The message to be sent as a reply.
 */
void Client::reply(const std::string &reply)
{
	this->write(":" + this->_server->getServerName() + " " + reply);
}

/**
 * @brief  Handles the client joining a channel.
 * 1. Adds the client to the channel.
 * 2. Stores the channel in the client's list of channels.
 * 3. If the channel was empty before joining, sets the client as the channel admin and operator.
 * 4. Constructs a string of all nicknames in the channel.
 * 5. Broadcasts a join message to the channel and sends back replies regarding the join,
 *    topic status, and list of names.
 * 
 * @param chan Pointer to the channel the client is joining.
 */
void Client::join(Channel *chan)
{
	chan->addClient(this);
	_user_chans.push_back(chan);

	if (chan->getNbrClients() == 1)
	{
		chan->setAdmin(this);
		chan->addOper(this);
	}

	std::string users;
	std::vector<std::string> nicknames = chan->getNickNames();
	for (std::vector<std::string>::iterator it = nicknames.begin(); it != nicknames.end(); it++)
		users.append(*it + " ");

	chan->broadcast(RPL_JOIN(getPrefix(), chan->getName()));

	reply(RPL_NOTOPIC(this->getNickName(), chan->getName()));
	reply(RPL_NAMREPLY(this->getNickName(), chan->getName(), users));
	reply(RPL_ENDOFNAMES(this->getNickName(), chan->getName()));
}

/**
 * @brief Handles the client leaving a channel.
 * 
 * The client is removed from the channel's list of users. If the client is being
 * kicked, the reason is provided. Otherwise, the client leaves voluntarily.
 * 
 * @param chan Pointer to the channel the client is leaving.
 * @param kicked Indicates if the client is being forcibly removed (1) or leaving voluntarily (0).
 * @param reason The reason for leaving, used if the client is not kicked.
 */
void Client::leave(Channel *chan, int kicked, std::string &reason)
{
	if (!_user_chans.empty())
		_user_chans.erase(this->_user_chans.begin() + this->_channelIndex(chan));
	if (!kicked)
		chan->removeClient(this, reason);
}

/**
 * @brief Sends a welcome message to the client upon successful registration.
 * 
 *  If the client is not registered, the function returns immediately.
 * Otherwise, it sends the following replies:
 *  - A welcome message with the client's nickname and prefix.
 *  - Host information with the server's name and version.
 *  - Server creation time.
 *  - Server information and supported features.
 *  - A Message of the Day (MOTD) header, the MOTD text, several lines of ASCII art,
 *    and an end-of-MOTD message.
 */
void Client::welcome()
{
	if (!this->isRegistered())
		return;

	reply(RPL_WELCOME(this->getNickName(), this->getPrefix()));
	reply(RPL_YOURHOST(this->getNickName(), this->_server->getServerName(), "0.1"));
	reply(RPL_CREATED(this->getNickName(), this->_server->getStartTime()));
	reply(RPL_MYINFO(this->getNickName(), this->_server->getServerName(), "0.1", "default", "iklot"));

	// TODO: Make a MOTD funtion(?).
	reply("375 " + this->getNickName() + " :- " + this->_server->getServerName() + " Message of the day -");
	reply("372 " + this->getNickName() + " :- Welcome to our IRC server!");

	reply("372 " + this->getNickName() + " :- a,  8a");
	reply("372 " + this->getNickName() + " :-  `8, `8)                            ,adPPRg,");
	reply("372 " + this->getNickName() + " :-   8)  ]8                        ,ad888888888b");
	reply("372 " + this->getNickName() + " :-  ,8' ,8'                    ,gPPR888888888888");
	reply("372 " + this->getNickName() + " :- ,8' ,8'                 ,ad8\"\"   `Y888888888P");
	reply("372 " + this->getNickName() + " :- 8)  8)              ,ad8\"\"        (8888888\"\"");
	reply("372 " + this->getNickName() + " :- 8,  8,          ,ad8\"\"            d888\"\"");
	reply("372 " + this->getNickName() + " :- `8, `8,     ,ad8\"\"            ,ad8\"\"");
	reply("372 " + this->getNickName() + " :-  `8, `\" ,ad8\"\"            ,ad8\"\"");
	reply("372 " + this->getNickName() + " :-     ,gPPR8b           ,ad8\"\"");
	reply("372 " + this->getNickName() + " :-    dP:::::Yb      ,ad8\"\"");
	reply("372 " + this->getNickName() + " :-    8):::::(8  ,ad8\"\"              jainavas");
	reply("372 " + this->getNickName() + " :-    Yb:;;;:d888\"\"                  jvidal-t");
	reply("372 " + this->getNickName() + " :-     \"8ggg8P\"                      mrubal-c");

	reply("376 " + this->getNickName() + " :End of MOTD command");
}

/**
 * @brief Finds the index of a given channel in the client's list of joined channels.
 * 
 * @param channel Pointer to the channel being searched.
 * @return unsigned long The index of the channel in the vector, or 0 if not found.
 */
unsigned long Client::_channelIndex(Channel *channel)
{
	unsigned long i = 0;
	std::vector<Channel *>::iterator it = this->_user_chans.begin();

	while (it != this->_user_chans.end())
	{
		if (*it == channel)
			return i;
		it++;
		i++;
	}
	return 0;
}
