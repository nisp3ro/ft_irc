#include "ft_irc.hpp"

// Global flags used to control server shutdown, toggle debug mode, and indicate that a signal has been received.
static bool exitFlag = false;       ///< Global flag to indicate when the server should shut down.
static bool debugFlag = false;      ///< Global flag to enable or disable debug mode.
static bool signalRecived = false;  ///< Global flag set when a signal is received (SIGINT or SIGQUIT).

/**
 * @brief Signal handler for SIGINT and SIGQUIT.
 *
 * This function handles operating system signals:
 * - For SIGINT (Ctrl+C), it prints a shutdown message, pauses for 2 seconds, and sets the exitFlag,
 *   indicating that the server should stop running.
 * - For SIGQUIT (Ctrl+\), it toggles the debug mode by switching the debugFlag state and prints a message showing the current state.
 * In both cases, signalRecived is set to true to notify the main loop that a signal was handled.
 *
 * @param signum The signal number received.
 */
static void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << "\rYou pressed Ctrl+C! The server will shut down. Goodbye!" << std::endl;
        sleep(2);
        exitFlag = true;
    }
    if (signum == SIGQUIT) {
        if (debugFlag == false) {
            std::cout << "\rDebug Mode On." << std::endl;
            debugFlag = true;
        } else if (debugFlag == true) {
            std::cout << "\rDebug Mode Off." << std::endl;
            debugFlag = false;
        }
    }
    signalRecived = true;
}

/**
 * @brief Server constructor.
 *
 * Initializes a new Server instance by setting the port and password, and by initializing various internal
 * parameters including the server name, start time, and the command handler. The clients file descriptors
 * pointer is set to NULL initially.
 *
 * @param port The port number on which the server will listen for incoming connections.
 * @param password The password required for clients to connect to the server.
 */
Server::Server(int port, std::string const &password) :
	_port(port),
	_password(password),
	_server_name(DEFAULT_SERVER_NAME),
	_start_time(dateString()),
	_clients_fds(NULL),
	_handler(CommandHandler(this))
{
	// No additional initialization code here.
}

/**
 * @brief Server copy constructor.
 *
 * Creates a new Server instance by deep-copying content from another Server.
 * This implementation creates deep copies of all clients, channels and file descriptors.
 *
 * @param src The source Server to copy from.
 */
Server::Server(const Server &src) :
	_port(src._port),
	_password(src._password),
	_server_name(src._server_name),
	_start_time(src._start_time),
	_server_socket(src._server_socket),
	_clients_fds(NULL),
	_handler(CommandHandler(this))
{
	// Deep copy of clients
	for (unsigned long i = 0; i < src._clients.size(); i++)
	{
		Client *client = new Client(*src._clients[i]);
		this->_clients.push_back(client);
	}
	
	// Deep copy of channels
	for (unsigned long i = 0; i < src._channels.size(); i++)
	{
		Channel *channel = new Channel(*src._channels[i]);
		this->_channels.push_back(channel);
	}
	
	// Deep copy of clients_fds if they exist
	if (src._clients_fds)
	{
		this->_clients_fds = new struct pollfd[src._clients.size() + 1];
		for (unsigned long i = 0; i < src._clients.size() + 1; i++)
		{
			this->_clients_fds[i].fd = src._clients_fds[i].fd;
			this->_clients_fds[i].events = src._clients_fds[i].events;
			this->_clients_fds[i].revents = src._clients_fds[i].revents;
		}
	}
}

/**
 * @brief Server assignment operator.
 *
 * Assigns the content of another Server to this one.
 * This implementation creates deep copies of all clients, channels and file descriptors.
 *
 * @param src The source Server to copy from.
 * @return Reference to the updated Server.
 */
Server &Server::operator=(const Server &src)
{
	if (this != &src)
	{
		// Clean up existing resources
		for (unsigned long i = 0; i < this->_clients.size(); i++)
			delete this->_clients[i];
		this->_clients.clear();
		
		for (unsigned long i = 0; i < this->_channels.size(); i++)
			delete this->_channels[i];
		this->_channels.clear();
		
		if (this->_clients_fds)
			delete[] this->_clients_fds;
		
		// Copy non-const member variables
		this->_password = src._password;
		this->_server_name = src._server_name;
		this->_start_time = src._start_time;
		this->_server_socket = src._server_socket;
		this->_clients_fds = NULL;  // Will be reallocated below if needed
		// Cannot copy _handler as it's not copyable (due to CommandHandler internal state)
		// Instead, we initialize it with our 'this' pointer
		new (&this->_handler) CommandHandler(this);
		
		// Deep copy of clients
		for (unsigned long i = 0; i < src._clients.size(); i++)
		{
			Client *client = new Client(*src._clients[i]);
			this->_clients.push_back(client);
		}
		
		// Deep copy of channels
		for (unsigned long i = 0; i < src._channels.size(); i++)
		{
			Channel *channel = new Channel(*src._channels[i]);
			this->_channels.push_back(channel);
		}
		
		// Deep copy of clients_fds if they exist
		if (src._clients_fds)
		{
			this->_clients_fds = new struct pollfd[src._clients.size() + 1];
			for (unsigned long i = 0; i < src._clients.size() + 1; i++)
			{
				this->_clients_fds[i].fd = src._clients_fds[i].fd;
				this->_clients_fds[i].events = src._clients_fds[i].events;
				this->_clients_fds[i].revents = src._clients_fds[i].revents;
			}
		}
	}
	return *this;
}

/**
 * @brief Server destructor.
 *
 * Cleans up the Server instance by deleting all dynamically allocated clients and channels,
 * as well as the array of client file descriptors used for polling.
 */
Server::~Server(void)
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
		delete this->_clients[i];
	for (unsigned long i = 0; i < this->_channels.size(); i++)
		delete this->_channels[i];
	delete [] this->_clients_fds;
}

/**
 * @brief Starts the server and listens for incoming connections.
 *
 * This function sets up the server's master socket with the following steps:
 * 1. Creates a master socket for IPv6 TCP connections.
 * 2. Sets socket options to allow multiple connections.
 * 3. Sets the socket to non-blocking mode.
 * 4. Binds the socket to the specified IPv6 address and port.
 * 5. Puts the socket into listening mode.
 * 6. Registers signal handlers for SIGINT and SIGQUIT for graceful shutdown and toggling debug mode.
 * 7. Enters a loop waiting for socket activity until the exitFlag becomes true.
 *
 * If any step fails (socket creation, binding, or listening), an error message is printed and the function returns.
 */
void Server::listen(void)
{
	struct sockaddr_in6 address;

	// Create a master socket for incoming connections.
	this->_server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (this->_server_socket == 0)
	{
		std::cout << "Error: Socket creation failed." << std::endl;
		return;
	}

	// Set the master socket to allow multiple connections.
	int opt = 1;
	if (setsockopt(this->_server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		std::cout << "Error: Can't set socket options." << std::endl;
		return;
	}

	// Set the socket to non-blocking mode.
	this->_setNonBlocking(this->_server_socket);

	// Configure the IPv6 address structure to accept connections on any address.
	const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
	address.sin6_family = AF_INET6;
	address.sin6_addr = in6addr_any;
	address.sin6_port = htons(this->_port);
	address.sin6_flowinfo = 0;
	address.sin6_scope_id = 0;

	// Bind the socket to the address and port.
	if (bind(this->_server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cout << "Error: Can't bind socket." << std::endl;
		close(this->_server_socket);
		return;
	}

	std::cout << "Starting ircserv on port " << this->_port << std::endl;

	// Listen on the socket with a backlog of 32 connections.
	if (::listen(this->_server_socket, 32) < 0)
	{
		std::cout << "Error: Can't listen on socket." << std::endl;
		return;
	}

	std::cout << "Waiting for connections ..." << std::endl;
	std::cout << "Press Ctrl + \\ for debug mode." << std::endl;
	std::cout << "Press Ctrl + C to close the server." << std::endl;

	// Construct the initial poll file descriptors array.
	this->_constructFds();

	// Register signal handlers for SIGINT and SIGQUIT.
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);

	// Main loop: wait for socket activity until exitFlag becomes true.
	while (exitFlag == false)
		this->_waitActivity();
}

/**
 * @brief Waits for activity on any of the connected sockets.
 *
 * Uses the poll() function to monitor the master socket and all client sockets for incoming data.
 * If poll() detects activity:
 * - If the activity is on the master socket, it accepts new connections.
 * - If the activity is on a client socket, it processes the received data.
 * Any errors during polling are reported unless caused by a received signal.
 */
void Server::_waitActivity(void)
{
	// Wait indefinitely (-1 timeout) for activity on any socket.
	int rc = poll(this->_clients_fds, this->_clients.size() + 1, -1);
	if (rc < 0 && signalRecived == false)
		std::cout << "Error: Can't look for socket(s) activity." << std::endl;
	if (signalRecived == true)
		signalRecived = false;

	// Loop through the master socket and client sockets to check for activity.
	for (unsigned long i = 0; i < this->_clients.size() + 1; i++)
	{
		// Skip if no events occurred on this socket.
		if (this->_clients_fds[i].revents == 0)
			continue;

		// If the activity is on the master socket, accept new connections.
		if (this->_clients_fds[i].fd == this->_server_socket)
			this->_acceptConnection();
		// Otherwise, if activity is detected on a client socket (client sockets start at index 1).
		else if (i > 0)
		{
			Client *client = this->_clients[i - 1];
			this->_receiveData(client);
		}
	}
}

/**
 * @brief Accepts incoming connections on the master socket.
 *
 * This function continuously accepts new client connections until no more pending connections exist.
 * For each accepted connection, it extracts the client's IP address and port, and then calls addClient()
 * to handle the new connection.
 */
void Server::_acceptConnection(void)
{
	int socket;

	do {
		struct sockaddr_in6 address;
		socklen_t addrlen = sizeof(address);

		// Accept a new connection.
		socket = accept(this->_server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		if (socket < 0)
		{
			// If no connection is pending, or if an error occurs that is not due to non-blocking mode.
			if (errno != EWOULDBLOCK)
				std::cout << "Error: Failed to accept connection." << std::endl;
			break;
		}

		// Add the new client using its socket, IP address (converted via ft_inet_ntop6), and port.
		this->addClient(socket, ft_inet_ntop6(&address.sin6_addr), ntohs(address.sin6_port));
	} while (socket != -1);
}

/**
 * @brief Receives data from a client.
 *
 * Reads data from the specified client's socket in a non-blocking manner using recv().
 * The data is buffered, and if a newline character ('\n') is detected, the complete message(s)
 * are split into separate commands and processed by the command handler.
 * Partial messages that do not end with a newline are stored in the client object for later completion.
 *
 * @param client Pointer to the Client object from which data is to be received.
 */
void Server::_receiveData(Client *client)
{
	char buffer[BUFFER_SIZE + 1];

	do {
		// Receive data from the client's socket.
		int ret = recv(client->getFD(), buffer, sizeof(buffer), 0);
		if (ret < 0)
		{
			// If the error is not due to no data being available (EWOULDBLOCK), remove the client.
			if (errno != EWOULDBLOCK)
			{
				std::cout << "Error: recv() failed for fd " << client->getFD();
				this->delClient(client->getFD());
			}
			break;
		}
		else if (!ret)
		{
			// If no bytes were received, the connection has been closed.
			this->delClient(client->getFD());
			break;
		}
		else
		{
			// Null-terminate the received data and convert it to a std::string.
			buffer[ret] = '\0';
			std::string buff = buffer;

			// If the message ends with a newline, split the message into commands.
			if (buff.at(buff.size() - 1) == '\n') {
				std::vector<std::string> cmds = ft_split(client->getPartialRecv() + buff, '\n');
				client->setPartialRecv("");

				// Process each command by passing it to the command handler.
				for (std::vector<std::string>::iterator it = cmds.begin(); it != cmds.end(); it++)
					this->_handleMessage(*it, client);
			}
			else
			{
				// If the data does not end with a newline, store it for the next read.
				client->setPartialRecv(client->getPartialRecv() + buff);
				if (debugFlag)
					std::cout << "partial recv(" << client->getFD() << "): " << buff << std::endl;
			}
		}
	} while (TRUE);
}

/**
 * @brief Sets the specified file descriptor to non-blocking mode.
 *
 * Uses the fcntl system call to add the O_NONBLOCK flag to the file descriptor.
 * If the operation fails and the file descriptor is not the master server socket,
 * the corresponding client is removed.
 *
 * @param fd The file descriptor to be set to non-blocking mode.
 */
void Server::_setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "Error: Can't set socket to non-blocking." << std::endl;
		if (fd != this->_server_socket)
			this->delClient(fd);
	}
}

/**
 * @brief Sends a message to a client.
 *
 * Appends a newline character to the message if it is not already present.
 * Sends the message via the send() system call and prints debug information if debug mode is enabled.
 * Reports an error if the number of bytes sent does not match the message length.
 *
 * @param message The message to be sent.
 * @param client_fd The file descriptor of the target client.
 * @return ssize_t The number of bytes successfully sent.
 */
ssize_t Server::send(std::string message, int client_fd) const
{
	if (message[message.size() - 1] != '\n')
		message += "\n";

	if (debugFlag)
		std::cout << "send(" << client_fd << "): " << message;

	ssize_t sent_size = ::send(client_fd, message.c_str(), message.length(), 0);
	if (sent_size != (ssize_t) message.length())
		std::cout << "Error: The message has not been sent entirely." << std::endl;
	return sent_size;
}

/**
 * @brief Broadcasts a message to all connected clients.
 *
 * Iterates over all clients in the server's client list and sends the specified message to each.
 * This is useful for messages that need to be delivered to every connected client.
 *
 * @param message The message to be broadcast.
 */
void Server::broadcast(std::string message) const
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		this->send(message, this->_clients[i]->getFD());
	}
}

/**
 * @brief Broadcasts a message to all clients except the specified one.
 *
 * Iterates over all clients in the server's client list and sends the specified message to each,
 * excluding the client with the given file descriptor.
 *
 * @param message The message to be broadcast.
 * @param exclude_fd The file descriptor of the client to exclude.
 */
void Server::broadcast(std::string message, int exclude_fd) const
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i]->getFD() != exclude_fd)
			this->send(message, this->_clients[i]->getFD());
	}
}

/**
 * @brief Adds a new client to the server.
 *
 * Adjusts the provided IP address string if it uses IPv6-mapped IPv4 format, creates a new Client object,
 * sets the client's socket to non-blocking mode, and updates the poll file descriptors array.
 * If debug mode is enabled, prints connection details.
 *
 * @param socket The socket file descriptor of the new client.
 * @param ip The IP address of the new client.
 * @param port The port number of the new client.
 * @return int The total number of connected clients after adding the new one.
 */
int Server::addClient(int socket, std::string ip, int port)
{
	std::string newip = ip;
	if (newip.find("::ffff:") != std::string::npos)
		newip = newip.substr(7);
	else if (newip.find("::") != std::string::npos)
		newip = newip.substr(2);

	if (newip.empty() || newip == "1")
		newip = "127.0.0.1";

	this->_clients.push_back(new Client(this, socket, newip, port));
	this->_setNonBlocking(socket);
	this->_constructFds();
	if (debugFlag)
		std::cout << "* New connection {fd: " << socket
		          << ", ip: " << ip
		          << ", port: " << port
		          << "}" << std::endl;
	return this->_clients.size();
}

/**
 * @brief Removes a client from the server.
 *
 * Searches for the client with the given socket file descriptor in the client list,
 * removes the client from all channels they are part of, deletes the Client object,
 * updates the poll file descriptors array, and closes the client's socket.
 *
 * @param socket The socket file descriptor of the client to be removed.
 * @return int The total number of connected clients after removal.
 */
int Server::delClient(int socket)
{
	std::string emptyString = "";

	for (unsigned long client = 0; client < this->_clients.size(); client++)
	{
		if (this->_clients[client]->getFD() == socket)
		{
			if (debugFlag)
				std::cout << "* Closed connection {fd: " << this->_clients[client]->getFD()
				          << ", ip: " << this->_clients[client]->getHostName()
				          << ", port: " << this->_clients[client]->getPort()
				          << "}" << std::endl;

			// Remove the client from all channels they are a member of.
			for (unsigned long chan = 0; chan < this->_channels.size(); chan++)
			{
				if (this->_channels[chan]->isInChannel(this->_clients[client]))
					this->_channels[chan]->removeClient(this->_clients[client], emptyString);
			}

			this->_clients.erase(this->_clients.begin() + client);
			break;
		}
	}
	this->_constructFds();
	close(socket);
	return this->_clients.size();
}

/**
 * @brief Retrieves a client based on its file descriptor.
 *
 * Iterates through the list of connected clients and returns a pointer to the Client object
 * with a matching file descriptor. Returns NULL if no such client is found.
 *
 * @param fd The file descriptor of the client to retrieve.
 * @return Client* Pointer to the matching Client object, or NULL if not found.
 */
Client *Server::getClient(int fd)
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i]->getFD() == fd)
			return this->_clients[i];
	}
	return NULL;
}

/**
 * @brief Retrieves a client based on its nickname.
 *
 * Iterates through the list of connected clients and returns a pointer to the Client object
 * that has a matching nickname. Returns NULL if no client with the given nickname is found.
 *
 * @param nickname The nickname to search for.
 * @return Client* Pointer to the matching Client object, or NULL if not found.
 */
Client *Server::getClient(const std::string &nickname)
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
 * @brief Reconstructs the array of pollfd structures for socket polling.
 *
 * Deletes the previous pollfd array (if any) and creates a new array that includes the master server socket
 * as well as all client sockets. This function is called whenever a client is added or removed to ensure
 * that the poll() function monitors the correct set of file descriptors.
 */
void Server::_constructFds(void)
{
	if (this->_clients_fds)
		delete [] this->_clients_fds;
	this->_clients_fds = new struct pollfd[this->_clients.size() + 1];

	// The first pollfd corresponds to the master server socket.
	this->_clients_fds[0].fd = this->_server_socket;
	this->_clients_fds[0].events = POLLIN;
	this->_clients_fds[0].revents = 0;

	// Add each client socket to the pollfd array.
	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		this->_clients_fds[i + 1].fd = this->_clients[i]->getFD();
		this->_clients_fds[i + 1].events = POLLIN;
		this->_clients_fds[i + 1].revents = 0;
	}
}

/**
 * @brief Retrieves a channel by its name.
 *
 * Iterates through the list of channels maintained by the server and returns a pointer to the Channel
 * object that has a matching name. Returns NULL if no such channel exists.
 *
 * @param name The name of the channel to retrieve.
 * @return Channel* Pointer to the matching Channel object, or NULL if not found.
 */
Channel *Server::getChannel(const std::string &name)
{
	std::vector<Channel *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++)
		if (it.operator*()->getName() == name)
			return it.operator*();

	return NULL;
}

/**
 * @brief Creates a new channel.
 *
 * Allocates and initializes a new Channel object with the specified name and password.
 * The client creating the channel is also added as the first member (and typically the operator).
 * The new channel is then added to the server's list of channels.
 *
 * @param name The name of the channel to create.
 * @param password The password for the channel.
 * @param client Pointer to the Client object creating the channel.
 * @return Channel* Pointer to the newly created Channel object.
 */
Channel *Server::createChannel(const std::string &name, std::string const &password, Client *client)
{
	Channel *channel = new Channel(name, password, client, this);
	_channels.push_back(channel);

	return channel;
}

/**
 * @brief Broadcasts a message to all clients in a specific channel.
 *
 * Retrieves the list of clients from the specified channel and sends the message
 * to each client's socket using the server's send() method.
 *
 * @param message The message to be broadcast.
 * @param channel Pointer to the Channel object whose clients will receive the message.
 */
void Server::broadcastChannel(std::string message, Channel const *channel) const
{
	std::vector<Client *> clients = channel->getChanClients();

	for (unsigned long i = 0; i < clients.size(); i++)
		this->send(message, clients[i]->getFD());
}

/**
 * @brief Broadcasts a message to all clients in a specific channel except one.
 *
 * Retrieves the list of clients from the specified channel and sends the message to each client's socket,
 * excluding the client with the specified file descriptor.
 *
 * @param message The message to be broadcast.
 * @param exclude_fd The file descriptor of the client to be excluded from receiving the message.
 * @param channel Pointer to the Channel object whose clients will receive the message.
 */
void Server::broadcastChannel(std::string message, int exclude_fd, Channel const *channel) const
{
	std::vector<Client *> clients = channel->getChanClients();

	for (unsigned long i = 0; i < clients.size(); i++)
		if (clients[i]->getFD() != exclude_fd)
			this->send(message, clients[i]->getFD());
}

/**
 * @brief Handles an incoming message from a client.
 *
 * If debug mode is enabled, prints the message along with the client's file descriptor.
 * Then, delegates the message to the command handler (_handler) to parse and execute
 * the appropriate command based on the message content.
 *
 * @param message The message received from the client.
 * @param client Pointer to the Client object that sent the message.
 */
void Server::_handleMessage(std::string const message, Client *client)
{
	if (debugFlag)
		std::cout << "recv(" << client->getFD() << "): " << message << std::endl;

	this->_handler.invoke(client, message);
}

/**
 * @brief Retrieves a list of all client nicknames connected to the server.
 *
 * Iterates through the list of connected clients and collects each client's nickname into a vector.
 *
 * @return std::vector<std::string> A vector containing the nicknames of all connected clients.
 */
std::vector<std::string> Server::getNickNames()
{
	std::vector<std::string> nicknames;
	std::vector<Client *>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		nicknames.push_back((*it)->getNickName());
		it++;
	}
	return nicknames;
}
