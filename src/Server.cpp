#include "ft_irc.hpp"

static bool exitFlag = false;
static bool debugFlag = false;
static bool signalRecived = false;

static void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << "\rYou pressed Ctrl+C! The server will shut down. Goodbye!" << std::endl;
		sleep(2);
        exitFlag = true;
    }
	if (signum == SIGQUIT)
	{
		if (debugFlag == false) {
		std::cout << "\rDebug Mode On." << std::endl;
        debugFlag = true;
		}
		else  if (debugFlag == true) {
		std::cout << "\rDebug Mode Off." << std::endl;
        debugFlag = false;
		}
	}
	signalRecived = true;
}

Server::Server(int port, std::string const &password) :
	_port(port),
	_password(password),
	_server_name(DEFAULT_SERVER_NAME),
	_start_time(dateString()),
	_clients_fds(NULL),
	_handler(CommandHandler(this))
{

}

Server::~Server(void)
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
		delete this->_clients[i];
	for (unsigned long i = 0; i < this->_channels.size(); i++)
		delete this->_channels[i];
	delete [] this->_clients_fds;
}

void	Server::listen(void)
{
	struct sockaddr_in6 address;

	//create a master socket
	this->_server_socket = socket(AF_INET6, SOCK_STREAM , IPPROTO_TCP);
	if(this->_server_socket == 0)
	{
		std::cout << "Error: Socket creation failed." << std::endl;
		return;
	}

	//set master socket to allow multiple connections
	int opt = 1;
	if(setsockopt(this->_server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		std::cout << "Error: Can't set socket options." << std::endl;
		return;
	}

	this->_setNonBlocking(this->_server_socket);

	//type of socket created
	const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
	address.sin6_family = AF_INET6;
	address.sin6_addr = in6addr_any;
	address.sin6_port = htons(this->_port);
	address.sin6_flowinfo = 0;
	address.sin6_scope_id = 0;

	if (bind(this->_server_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		std::cout << "Error: Can't bind socket." << std::endl;
		close(this->_server_socket);
		return;
	}

	std::cout << "Starting ircserv on port " << this->_port << std::endl;

	// listen
	if (::listen(this->_server_socket, 32) < 0)
	{
		std::cout << "Error: Can't listen on socket." << std::endl;
		return;
	}

	std::cout << "Waiting for connections ..." << std::endl;
	std::cout << "Press Ctrl + \\ for debug mode." << std::endl;
	std::cout << "Press Ctrl + C for close the server." << std::endl;
	this->_constructFds();
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	while (exitFlag == false)
		this->_waitActivity();
}

void	Server::_waitActivity(void)
{
	// wait for an activity in a socket
	int rc = poll(this->_clients_fds, this->_clients.size() + 1, -1);
	if (rc < 0 && signalRecived == false)
		std::cout << "Error: Can't look for socket(s) activity." << std::endl;
	if (signalRecived == true)
		signalRecived = false;
	// loop in every client socket for a connection
	for(unsigned long i=0; i < this->_clients.size() + 1; i++)
	{
		// revents will be != 0 if there is an activity on the socket
		if(this->_clients_fds[i].revents == 0)
			continue;

		// if something append in the master socket, it's an incomming connection
		if (this->_clients_fds[i].fd == this->_server_socket)
			this->_acceptConnection();
		else if (i > 0) // should be always true, because the first socket is the server socket
		{
			Client *client = this->_clients[i - 1];
			this->_receiveData(client);
		}
	}
}

void	Server::_acceptConnection(void)
{
	int	socket;

	do {
		struct sockaddr_in6 address;
		socklen_t addrlen = sizeof(address);

		socket = accept(this->_server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		if (socket < 0)
		{
			if (errno != EWOULDBLOCK)
				std::cout << "Error: Failed to accept connection." << std::endl;
			break;
		}
		this->addClient(socket, ft_inet_ntop6(&address.sin6_addr), ntohs(address.sin6_port));
	} while (socket != -1);
}

void	Server::_receiveData(Client *client)
{
	char	buffer[BUFFER_SIZE + 1];

	do {
		int ret = recv(client->getFD(), buffer, sizeof(buffer), 0);
		if (ret < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				std::cout << "Error: recv() failed for fd " << client->getFD();
				this->delClient(client->getFD());
			}
			// no more data to receive
			break;
		}
		else if (!ret)
		{
			this->delClient(client->getFD());
			break;
		}
		else
		{
			buffer[ret] = '\0';
			std::string buff = buffer;

			if (buff.at(buff.size() - 1) == '\n') {
				std::vector<std::string> cmds = ft_split(client->getPartialRecv() + buff, '\n');
				client->setPartialRecv("");

				for (std::vector<std::string>::iterator it = cmds.begin(); it != cmds.end(); it++)
					this->_handleMessage(*it, client);
			}
			else
			{
				client->setPartialRecv(client->getPartialRecv() + buff);
				if (debugFlag)
					std::cout << "partial recv(" << client->getFD() << "): " << buff << std::endl;
			}
		}
	} while(TRUE);
}

void	Server::_setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "Error: Can't set socket to non-blocking." << std::endl;
		if (fd != this->_server_socket)
			this->delClient(fd);
	}
}

ssize_t	Server::send(std::string message, int client_fd) const
{
	if (message[message.size() - 1] != '\n')
		message += "\n";

	if (debugFlag)
		std::cout << "send(" << client_fd << "): " << message;

	ssize_t	sent_size = ::send(client_fd, message.c_str(), message.length(), 0);
	if(sent_size != (ssize_t) message.length())
		std::cout << "Error: The message has not been sent entirely." << std::endl;
	return (sent_size);
}

void	Server::broadcast(std::string message) const
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		this->send(message, this->_clients[i]->getFD());
	}
}

void	Server::broadcast(std::string message, int exclude_fd) const
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i]->getFD() != exclude_fd)
			this->send(message, this->_clients[i]->getFD());
	}
}

int	Server::addClient(int socket, std::string ip, int port)
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

int	Server::delClient(int socket)
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

			// Remove from channels
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

Client	*Server::getClient(int fd)
{
	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i]->getFD() == fd)
			return this->_clients[i];
	}
	return NULL;
}

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

void	Server::_constructFds(void)
{
	if (this->_clients_fds)
		delete [] this->_clients_fds;
	this->_clients_fds = new struct pollfd[this->_clients.size() + 1];

	this->_clients_fds[0].fd = this->_server_socket;
	this->_clients_fds[0].events = POLLIN;
	this->_clients_fds[0].revents = 0;

	for (unsigned long i = 0; i < this->_clients.size(); i++)
	{
		this->_clients_fds[i + 1].fd = this->_clients[i]->getFD();
		this->_clients_fds[i + 1].events = POLLIN;
		this->_clients_fds[i + 1].revents = 0;
	}
}

Channel *Server::getChannel(const std::string &name)
{
	std::vector<Channel *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++)
		if (it.operator*()->getName() == name)
			return it.operator*();

	return NULL;
}

Channel *Server::createChannel(const std::string &name, std::string const &password, Client *client)
{
	Channel *channel = new Channel(name, password, client, this);
	_channels.push_back(channel);

	return channel;
}

void	Server::broadcastChannel(std::string message, Channel const *channel) const
{
	std::vector<Client *> clients = channel->getChanClients();

	for (unsigned long i = 0; i < clients.size(); i++)
		this->send(message, clients[i]->getFD());
}

void	Server::broadcastChannel(std::string message, int exclude_fd, Channel const *channel) const
{
	std::vector<Client *> clients = channel->getChanClients();

	for (unsigned long i = 0; i < clients.size(); i++)
		if (clients[i]->getFD() != exclude_fd)
			this->send(message, clients[i]->getFD());
}


void	Server::_handleMessage(std::string const message, Client *client)
{
	if (debugFlag)
		std::cout << "recv(" << client->getFD() << "): " << message << std::endl;


	this->_handler.invoke(client, message);
}

std::vector<std::string>	Server::getNickNames()
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
