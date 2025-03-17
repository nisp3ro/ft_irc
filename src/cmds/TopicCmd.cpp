#include "ft_irc.hpp"

/**
 * @brief Constructs a new TopicCommand object.
 *
 * Initializes the TOPIC command handler by invoking the base Command constructor.
 *
 * @param server Pointer to the Server instance.
 */
TopicCommand::TopicCommand(Server *server) : Command(server) {}

/**
 * @brief Destroys the TopicCommand object.
 *
 * Cleans up any resources used by the TopicCommand object.
 */
TopicCommand::~TopicCommand() {}

/**
 * @brief Executes the TOPIC command.
 *
 * The TOPIC command can be used to query or set the topic of a channel.
 * Expected usage:
 * - To query the current topic: TOPIC <channel>
 * - To set a new topic: TOPIC <channel> :<new topic>
 *
 * This function performs the following:
 * 1. Validates that a channel name is provided.
 * 2. Retrieves the channel; if not found, sends ERR_NOSUCHCHANNEL.
 * 3. Checks if the client is a member; if not, sends ERR_NOTONCHANNEL.
 * 4. If only the channel name is provided, replies with the current topic (or RPL_NOTOPIC if none).
 * 5. If a new topic is provided, verifies privileges if the channel is topic-restricted,
 *    sets the new topic, and broadcasts the change.
 *
 * @param client Pointer to the Client issuing the TOPIC command.
 * @param arguments A vector of strings containing the command parameters.
 */
void TopicCommand::execute(Client *client, std::vector<std::string> arguments)
{
    if (arguments.empty() || arguments[0].empty())
    {
        client->reply(ERR_NEEDMOREPARAMS(client->getNickName(), "TOPIC"));
        return;
    }

    std::string channelName = arguments[0];
    Channel *channel = _server->getChannel(channelName);
    if (!channel)
    {
        client->reply(ERR_NOSUCHCHANNEL(client->getNickName(), channelName));
        return;
    }

    if (!channel->isInChannel(client))
    {
        client->reply(ERR_NOTONCHANNEL(client->getNickName(), channelName));
        return;
    }

    if (arguments.size() == 1)
    {
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty())
            client->reply(RPL_NOTOPIC(client->getNickName(), channelName));
        else
            client->reply(RPL_TOPIC(client->getNickName(), channelName, currentTopic));
    }
    else
    {
        if (channel->topicRestricted() && channel->getAdmin() != client && !channel->is_oper(client))
        {
            client->reply(ERR_CHANOPRIVSNEEDED(client->getNickName(), channelName));
            return;
        }

        std::string newTopic = arguments[1];
        if (newTopic[0] == ':')
            newTopic = newTopic.substr(1);
        channel->setTopic(newTopic);
        channel->broadcast(RPL_TOPIC(client->getPrefix(), channelName, newTopic));
    }
}
