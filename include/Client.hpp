#ifndef CLIENT_CLASS_H
# define CLIENT_CLASS_H

#include <iostream>
#include <vector>

class Channel;
class Server;

class Client
{
	private:
		int			_fd;
		std::string _hostname;
		int 		_port;
		bool		_correct_password;

		std::string _nickname;
		std::string _username;
		std::string _realname;

		std::vector<Channel *> _user_chans;

		std::string	_partial_recv;

		Server	*_server;

		unsigned long	_channelIndex(Channel *channel);
	public:
		Client(Server *server, int fd, std::string const &hostname, int port);
		~Client();

		// GETTERS

		bool 					isRegistered() const;
		int						getFD() const { return _fd; };
		std::string const 		&getHostName() const { return _hostname; };
		int 					getPort() const { return _port; };

		std::string const 		&getNickName() const { return _nickname; };
		std::string const 		&getUserName() const { return _username; };
		std::string const 		&getRealName() const { return _realname; };
		std::string const 		&getPartialRecv() const { return _partial_recv; };

		std::vector<Channel *> 	getUserChans() const { return _user_chans; };

		// SETTERS

		void 					setNickname(const std::string &nickname) { _nickname = nickname; };
		void 					setUsername(const std::string &username) { _username = username; };
		void 					setRealName(const std::string &realname) { _realname = realname; };
		void 					setPartialRecv(const std::string &partial_recv) { _partial_recv = partial_recv; };
		void					setCorrectPassword(bool correct_password) { _correct_password = correct_password; };

		// OTHER

		void 					write(const std::string &message) const;
		void 					reply(const std::string &reply);
		std::string 			getPrefix() const;
		void 					welcome();
		void					join(Channel *chan);
		void					leave(Channel *chan, int kicked, std::string &reason);
};

#endif
