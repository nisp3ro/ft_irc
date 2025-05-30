#ifndef COMMAND_CLASS_HPP
#define COMMAND_CLASS_HPP

#include <string>
#include <numeric>

class Server;
class Client;

class Command
{
	protected:
		Server *_server;
		bool _authRequired;

	public:
		explicit Command(Server *server, bool authRequired = true) : _server(server), _authRequired(authRequired){};
		virtual ~Command(){};

		bool authRequired() const { return _authRequired; };

		virtual void execute(Client *client, std::vector<std::string> arguments) = 0;
};

class NoticeCommand : public Command
{
	public:
		NoticeCommand(Server *server);
		~NoticeCommand();

		void execute(Client *client, std::vector<std::string> arguments);
	};

class PrivMsgCommand : public Command
{
	public:
		PrivMsgCommand(Server *server);
		~PrivMsgCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class PartCommand : public Command
{
	public:
		PartCommand(Server *server);
		~PartCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class QuitCommand : public Command
{
	public:
		QuitCommand(Server *server, bool authRequired);
		~QuitCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class JoinCommand : public Command
{
	public:
		JoinCommand(Server *server);
		~JoinCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class UserCommand : public Command
{
	public:
		UserCommand(Server *server, bool authRequired);
		~UserCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class NickCommand : public Command
{
	public:
		NickCommand(Server *server, bool authRequired);
		~NickCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class PassCommand : public Command
{
	public:
		PassCommand(Server *server, bool authRequired);
		~PassCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class KickCommand : public Command
{
	public:
		KickCommand(Server *server);
		~KickCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class PingCommand : public Command
{
	public:
		PingCommand(Server *server);
		~PingCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class PongCommand : public Command
{
	public:
		PongCommand(Server *server);
		~PongCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class ModeCommand : public Command
{
	public:
		ModeCommand(Server *server);
		~ModeCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class InvitCommand : public Command
{
	public:
		InvitCommand(Server *server);
		~InvitCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class WhoCommand : public Command
{
	public:
		WhoCommand(Server *server);
		~WhoCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class ListCommand : public Command
{
	public:
		ListCommand(Server *server);
		~ListCommand();

		void execute(Client *client, std::vector<std::string> arguments);
};

class TopicCommand : public Command
{
	public:
		TopicCommand(Server *server);
		~TopicCommand();
		
		void execute(Client *client, std::vector<std::string> arguments);
};

#endif
