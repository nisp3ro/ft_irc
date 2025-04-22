#ifndef SERVER_CLASS_H
# define SERVER_CLASS_H

# include <vector>
# include <iostream>

# include <stdio.h>
# include <string>
# include <fcntl.h>
# include <stdlib.h>
# include <ctime>
# include <csignal>
# include <cstring> 
# include <errno.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <sys/poll.h>
# include <netinet/in.h>
# include <sys/time.h>

# include "CommandHandler.hpp"

# define DEFAULT_SERVER_NAME "irc.42.fr"

class Client;

class Server {
private:
	const int				_port;
	std::string 			_password;
	std::vector<Client *>	_clients;
	std::vector<Channel *>	_channels;
	std::string				_server_name;
	std::string				_start_time;

	int						_server_socket;
	struct pollfd			*_clients_fds;
	CommandHandler			_handler;


	void					_waitActivity(void);
	void					_constructFds(void);
	void 					_setNonBlocking(int fd);
	void					_acceptConnection(void);
	void 					_receiveData(Client *client);
	void					_handleMessage(std::string const message, Client *client);

public:
	Server(int port, std::string const &password);
	Server(const Server &src);
	~Server(void);
	Server &operator =(const Server &src);

	// Server
	void			listen(void);
	ssize_t			send(std::string const message, int const client_fd) const;
	void			broadcast(std::string const message) const;
	void			broadcast(std::string const message, int const exclude_fd) const;
	void			broadcastChannel(std::string const message, Channel const *channel) const;
	void			broadcastChannel(std::string message, int exclude_fd, Channel const *channel) const;
	std::string&	getPassword() { return _password; };
	std::string&	getServerName() { return _server_name; };
	std::string&	getStartTime() { return _start_time; };
	// Client
	std::vector<std::string>	getNickNames();
	std::vector<Client *> 		getServClients() const { return _clients; };
	int							addClient(int const fd, std::string const ip, int const port);
	int							delClient(int fd);
	Client*						getClient(int fd);
	Client*						getClient(const std::string &nickname);
	// Channel
	Channel*					getChannel(std::string const &name);
	std::vector<Channel *>		getServChannels() const { return _channels; };
	std::vector<Channel *>&		getServChannels() { return _channels; };
	Channel* 					createChannel(std::string const &name, std::string const &password, Client *client);
	bool						removeChannel(Channel *channel);
};

#endif
