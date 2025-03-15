#pragma once

#include <iostream>

//ERROR REPLIES
#define ERR_NEEDMOREPARAMS(source, command)				"461 " + source + " " + command + " :Not enough parameters"
#define ERR_NOTONCHANNEL(source, channel)				"442 " + source + " " + channel + " :You're not on that channel"
#define ERR_USERNOTINCHANNEL(source, nickname, channel)	"441 " + source + " " + nickname + " " + channel + " :They aren't on that channel"
#define ERR_CHANNELISFULL(source, channel)				"471 " + source + " " + channel + " :Cannot join channel (+l)"
#define ERR_BADCHANNELKEY(source, channel)				"475 " + source + " " + channel + " :Cannot join channel (+k)"
#define ERR_INVITONLYCHAN(source, channel)				"473 " + source + " " + channel + " :Cannot join channel (+i)"
#define ERR_NOSUCHCHANNEL(source, channel)				"403 " + source + " " + channel + " :No such channel"
#define ERR_CHANOPRIVSNEEDED(source, channel)			"482 " + source + " " + channel + " :You're not channel operator"
#define ERR_NONICKNAMEGIVEN(source)						"431 " + source + " :Nickname not given"
#define ERR_NICKNAMEINUSE(source, nickname)				"433 " + source + " " + nickname + " :Nickname is already in use"
#define ERR_ALREADYREGISTERED(source)					"462 " + source + " :You may not reregister"
#define ERR_PASSWDMISMATCH(source)						"464 " + source + " :Password incorrect"
#define ERR_NOTREGISTERED(source)						"451 " + source + " :You have not registered"
#define ERR_UNKNOWNCOMMAND(source, command)				"421 " + source + " " + command + " :Unknown command"
#define ERR_USERONCHANNEL(source, target, channel)		"443 " + source + " " + target + " " + channel + " :is already on channel"
#define ERR_NOSUCHNICK(source, name)					"401 " + source + " " + name + " :No such nick/channel"
#define ERR_INVITEONLYCHAN(source, channel)				"473 " + source + " " + channel + " :Cannot join channel (+i)"

// NUMERIC REPLIES
#define RPL_WELCOME(source, prefix)										"001 " + source + " :Welcome to the Internet Relay Network " + prefix
#define RPL_YOURHOST(source, servername, version)						"002 " + source + " :Your host is " + servername + ", running version " + version
#define RPL_CREATED(source, date)										"003 " + source + " :This server was created " + date
#define RPL_MYINFO(source, servername, version, usermodes, chanmodes)	"004 " + source + " :" + servername + " " + version + " " + usermodes + " " + chanmodes

#define RPL_NAMREPLY(source, channel, users)			"353 " + source + " = " + channel + " :" + users
#define RPL_ENDOFNAMES(source, channel)					"366 " + source + " " + channel + " :End of /NAMES list."
#define RPL_INVITING(source, channel, target)			"341 " + source + " " + channel + " " + target

#define RPL_NOTOPIC(source, channel)					"331 " + source + " " + channel + " :No topic is set"
#define RPL_TOPIC(source, channel, topic)				"332 " + source + " " + channel + " :" + topic

#define RPL_WHOREPLY(source, channel, username, hostname, serverhostname, nickname, realname)	"352 " + source + " " + channel + " " + username + " " + hostname + " " + serverhostname + " " + nickname + " H :0 " + realname
#define RPL_ENDOFWHO(source, channel)					"315 " + source + " " + channel + " :End of WHO list"

#define RPL_LIST(source, channel, nbUsers, topic)		"322 " + source + " " + channel + " " + nbUsers + " :" + topic
#define RPL_LISTEND(source)					"323 " + source + " :End of LIST"

// COMMAND REPLIES

#define RPL_MODE(source, channel, modes, args)		":" + source + " MODE " + channel + " " + modes + " " + args
#define RPL_PING(source, token)						":" + source + " PONG :" + token
#define RPL_QUIT(source, message)					":" + source + " QUIT :" + message
#define RPL_JOIN(source, channel)					":" + source + " JOIN :" + channel
#define RPL_PART(source, channel)					":" + source + " PART " + channel
#define RPL_PART_REASON(source, channel, reason)	":" + source + " PART " + channel + " :" + reason
#define RPL_KICK(source, channel, target, reason)	":" + source + " KICK " + channel + " " + target + " :" + reason
#define RPL_PRIVMSG(source, target, message)		":" + source + " PRIVMSG " + target + " :" + message
#define RPL_NOTICE(source, target, message)			":" + source + " NOTICE " + target + " :" + message
#define RPL_INVITE(source, target, channel)			":" + source + " INVITE " + target + " :" + channel
