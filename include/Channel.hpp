#ifndef CHANNEL_CLASS_H
# define CHANNEL_CLASS_H

# include <vector>
# include <string>

class Client;
class Server;

class Channel
{
private:
	std::string _name;
	Client		*_admin;

	int 		_l; //max user in chan
	bool		_i; //invite-only chan
	std::string _k; //chan's key(paasword)

	std::vector<Client *> _clients;
	std::vector<Client *> _oper_clients;

	Server *_server;

	unsigned long	_clientIndex(std::vector<Client *> clients, Client *client);
public:
	Channel(std::string const &name, const std::string &password, Client *admin, Server *server);
	~Channel();

	// GETTERS

	Client						*getAdmin() const { return _admin; };
	std::string const 			&getName() const { return _name; };
	std::string const 			&getPassword() const { return _k; };
	int							getMaxUsers() const { return _l; };
	int							invitOnlyChan() { return _i; }

	Client*						getClient(const std::string &nickname);
	std::vector<Client *> 		getChanClients() const { return _clients; };
	std::vector<Client *> 		getChanOpers() const { return _oper_clients; };

	int							getNbrClients() const { return _clients.size(); };
	std::vector<std::string>	getNickNames();

	// SETTERS

	void						setAdmin(Client *client) { _admin = client; };
	void						setPassword(std::string k) { _k = k; };
	void						setMaxClients(int l) { _l = l; };
	void						setInviteOnly(bool active) { this->_i = active; };

	// OTHER

	void 						broadcast(std::string const &message);
	void 						broadcast(const std::string &message, Client *exclude);
	void 						removeClient(Client *client, std::string reason);
	void 						removeOper(Client *client);
	void						addClient(Client *client) { _clients.push_back(client); };
	void						addOper(Client *client) { _oper_clients.push_back(client); };
	void						kick(Client *client, Client *target, std::string reason);
	void						invit(Client *client, Client *target);
	int 						is_oper(Client *client);
	bool						isInChannel(Client *client);
};

#endif
