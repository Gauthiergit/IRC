#include "Server.hpp"
#include "Channel.hpp"
#include <cstdlib>
#include <stdlib.h>


void    Server::createChannel(std::string channelName, int i)
{
	Channel	newChannel(channelName);
	newChannel.addOperator(clients[i - 1]);
	addChannel(newChannel);
	clients[i - 1].addChannel(newChannel, 0);
	std::cout << GREEN << "Channel successfully created" << RESET << std::endl;
	// -------> tell irssi that this client is an OPERATOR
	std::string opeMsg = ":" + clients[i - 1].getHost() + " MODE " + channelName + " +o " + clients[i - 1].getNick() + "\r\n";
	send_mess(this->clients[i - 1].getFd(), opeMsg.c_str(), opeMsg.length(), 0);
	// std::string msg_list = ":" + clients[i - 1].getHost() + " 353 " + clients[i - 1].getNick() + " = " + channelName + " :@" + clients[i - 1].getNick() + "\r\n";
	// std::string endMSG = ":" + clients[i - 1].getHost() + " 366 " + clients[i - 1].getNick()+ " " + channelName + " :End of /NAMES list.\r\n";
	// send_mess(this->clients[i - 1].getFd(), msg_list.c_str(), msg_list.length(), 0);
	// send_mess(this->clients[i - 1].getFd(), endMSG.c_str(), endMSG.length(), 0);
}

void    Server::join(std::string channelName, std::string key, int i)
{
	std::string nick = clients[i - 1].getNick();
	std::string user = clients[i - 1].getUser();
	Channel* channel = getChannel(channelName);
	// -----------check if client already in channel-------------
	Client* operator_already_present = channel->getOperator(nick);
	Client* basic_client_already_present = channel->getBasicClient(nick);
	if (!operator_already_present && !basic_client_already_present)
	{
		// -----------check if channel is in mode invite only and client is invited-------------
		if ((channel->getInvite() == true && clients[i - 1].findChannel(channelName) == false))
		{
			std::string error = ":" + clients[i - 1].getHost() + " 473 " + clients[i - 1].getNick() + " " + channelName + " :Access allowed only on invite.\r\n";
			send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
			return;
		}
		// -----------check if client has the key if there is a key and the client isn't invited-------------
		if (channel->getKey() != "" && channel->getKeyAccess() == true)
		{
			if (key == "" || channel->getKey() != key)
			{
				std::string error = ":" + clients[i - 1].getHost() + " 475 " + clients[i - 1].getNick() + " " + channelName + " :Cannot join channel (+k) - bad key\r\n";
				send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
				return;
			}
		}
		else if (channel->getKey() != "" && channel->getKeyAccess() == false)
			channel->setKeyAccess(true);
		// -----------check if there is a limit of clients-------------
		size_t client_nb = channel->countOperators() + channel->countBasicClients();
		if (channel->getLimit() && client_nb >= channel->getLimitValue())
		{
			std::string error = ":" + clients[i - 1].getHost() + " 471 " + clients[i - 1].getNick() + " " + channelName + " :Cannot join channel (+l) - channel is full\r\n";
			send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
			return;
		}
		channel->addBasicClient(clients[i - 1]);
		clients[i - 1].removeChannel(channelName, 0);
		clients[i - 1].addChannel(*getChannel(channelName), 0);
		// -------> send to all clients in the channel "nick has joined #channel"
		std::string welcomeMessage;
		std::string cmd = "JOIN";
		welcomeMessage = ":" + nick + "!" + user + "@irc.example.com " + cmd + " " + channelName + "\r\n";
		channel->sendToAllClientsChannel(welcomeMessage, 0, 0);
		if (channel->getTopic() != ""){
			std::string msg = ":" + channel->getOpeWhoChangeTopic() + "!" + channel->getOpeWhoChangeTopic() + "@" + clients[i - 1].getHost() + " TOPIC " + channelName + " " + channel->getTopic() + "\r\n";
			send_mess(clients[i - 1].getFd(), msg.c_str(), msg.length(), 0);
		}
		// -------> send to all clients, the list of all other members of this #channel
		// std::string basic_clients = channel->getListBasicClients();
		// std::string operators = channel->getListOperators();
		// std::string client_list = operators + " " + basic_clients;
		// std::string msg_list = ":" + clients[i - 1].getHost() + " 353 " + nick + " = " + channelName + " :" + client_list + "\r\n";
		// std::string endMSG = ":" + clients[i - 1].getHost() + " 366 " + nick + " " + channelName + " :End of /NAMES list.\r\n";
		// channel->sendToAllClientsChannel(msg_list, 0, 0);
		// channel->sendToAllClientsChannel(endMSG, 0, 0);
	}
}

void    Server::joinChannel(std::vector<std::string> &cmd, int i)
{
	if (cmd.size() < 2)
	{
		notEnoughParameters(cmd, i);
		return;
	}
	std::istringstream issChan(cmd[1]);
    std::string channelName;
	std::istringstream issKey;
    std::string key;
	if (cmd.size() == 3)
		issKey.str(cmd[2]);
	while (std::getline(issChan, channelName, ','))
	{
		Channel *cha = getChannel(channelName);
		if (cmd.size() == 2)
		{
			if (!cha)
        		createChannel(channelName, i);
			else
				join(channelName, "", i);
		}
		else
		{
			if (cha && cha->getKeyAccess() && std::getline(issKey, key, ','))
				join(channelName, key, i);
			else if (cha)
				join(channelName, "", i);
			else
				createChannel(channelName, i);
		}
	}
}
