#include "Server.hpp"
#include "Channel.hpp"

/*---------------------------------------------------||||CLIENTS COMMANDS||||---------------------------------------------------*/

void Server::kick(std::vector<std::string> &cmd, int i)
{
	if (cmd.size() < 3)
	{
		notEnoughParameters(cmd, i);
		return;
	}
	std::string reason = ":no reason";
	if (cmd.size() > 4)
		reason = join_cmd(cmd, " ", 3);
	std::string msg = ":" + clients[i - 1].getNick() + "!" + clients[i - 1].getUser() + "@" + clients[i - 1].getHost() + " " +  cmd[0] + " " + cmd[1] + " " + cmd[2] + " " + reason + "\r\n";
	Channel* channel = getChannel(cmd[1]);
	if (channel != NULL)
	{
		Client* ope = channel->getOperator(clients[i - 1].getNick());
		if (ope != NULL)
		{
			if (client_exist(cmd[2]))
			{
				if (client_already_in_channel(cmd[2], channel->getName()))
				{
					Client *client_found = channel->getOperator(cmd[2]);
					if (client_found && cmd[2] != ope->getNick())
					{
						channel->sendToAllClientsChannel(msg, 0, 0);
						getClient(cmd[2])->removeChannel(channel->getName(), 1);
						channel->removeOperator(cmd[2]);
						return;
					}
					client_found = channel->getBasicClient(cmd[2]);
					if (client_found)
					{
						channel->sendToAllClientsChannel(msg, 0, 0);
						getClient(cmd[2])->removeChannel(channel->getName(), 1);
						channel->removeBasicClient(cmd[2]);
						return;
					}
				}
				else
					not_in_channel(cmd, i);
			}
			else
				client_doesnt_exist(cmd, i);
		}
		else
			notChannelOperator(cmd, i);
	}
	else
		nonExistentChannel(cmd,i);
}

void Server::invite(std::vector<std::string> &cmd, int i)
{
	if (cmd.size() < 3)
	{
		notEnoughParameters(cmd, i);
		return;
	}
	Channel* channel = getChannel(cmd[2]);
	if (channel != NULL)
	{
		Client* ope = channel->getOperator(clients[i - 1].getNick());
		if (ope != NULL)
		{
			if (client_exist(cmd[1]))
			{
				if (!client_already_in_channel(cmd[1], channel->getName()))
				{
					std::string msg = ":" + clients[i - 1].getNick() + "!" + clients[i - 1].getUser() + "@" + clients[i - 1].getHost() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + "\r\n";
					for (size_t j = 0; j < clients.size(); j++)
					{
						if (clients[j].getNick() == cmd[1])
						{
							channel->setKeyAccess(false);
							clients[j].addChannel(*channel, 1);
							send_mess(clients[j].getFd(), msg.c_str(), msg.length(), 0);
							return;
						}
					}
				}
				else
					in_channel(cmd, i);
			}
			else
				client_doesnt_exist(cmd, i);
		}
		else
			notChannelOperator(cmd, i);
	}
	else
		nonExistentChannel(cmd, i);
}

void Server::topic(std::vector<std::string> &cmd, int i)
{
	if (cmd.size() < 2)
	{
		notEnoughParameters(cmd, i);
		return;
	}
	Channel* channel = getChannel(cmd[1]);
	if (channel != NULL)
	{
		Client* ope = channel->getOperator(clients[i - 1].getNick());
		if (channel->getTopicAccess() && !ope)
		{
			notChannelOperator(cmd, i);
			return;
		}
		if (cmd.size() == 2 && channel->getTopic() != "")
		{
			std::string msg = ":" + clients[i - 1].getHost() + " 332 " + clients[i - 1].getNick() + " " + cmd[1] + " " + channel->getTopic() + "\r\n";
			std::string next_msg =  ":" + clients[i - 1].getHost() + " 332 " + clients[i - 1].getNick() + " " + cmd[1] + " " + channel->getOpeWhoChangeTopic() + " " + 
				to_string(channel->getTopicTimestamp()) + "\r\n";
			send_mess(clients[i - 1].getFd(), msg.c_str(), msg.length(), 0);
			send_mess(clients[i - 1].getFd(), next_msg.c_str(), next_msg.length(), 0);
		}
		else if (cmd.size() == 2 && channel->getTopic() == "")
		{
			std::string msg = ":" + clients[i - 1].getHost() + " 331 " + clients[i - 1].getNick() + " " + cmd[1] + " :No topic is set.\r\n";
			send_mess(clients[i - 1].getFd(), msg.c_str(), msg.length(), 0);
		}
		else
		{
			std::string _topic = cmd[2];
			if (cmd.size() > 3)
				_topic = join_cmd(cmd, " ", 2);
			std::string msg = ":" + clients[i - 1].getNick() + "!" + clients[i - 1].getUser() + "@" + clients[i - 1].getHost() + " " + cmd[0] + " " + cmd[1] + " " + _topic + "\r\n";
			channel->setTopicTimestamp();
			channel->setOpeWhoChangeTopic(clients[i - 1].getNick());
			channel->sendToAllClientsChannel(msg, 0, 0);
			channel->setTopic(_topic);
		}
	}
	else
		nonExistentChannel(cmd, i);
}

void Server::nick(std::vector<std::string> &cmd, int i)
{
	Client *him = &clients[i - 1];
	if (cmd.size() < 2){
		std::string error = ":" + him->getHost() + " 432 " + him->getNick() + " <invalid_nick> :Erroneous nickname\r\n";
		send_mess(this->fds[i].fd, error.c_str(), error.length(), 0);
	}
	if (verif_nick(cmd[1], i)){
		std::string answer = ":" + him->getNick() + "!" + him->getNick() + "@" + him->getHost() + " NICK :" + cmd[1] +"\r\n";
		him->setNick(cmd[1]);
		std::vector<Channel> all_cha = him->getInChannels();
		for (size_t j = 0; j < all_cha.size(); ++j)
			all_cha[j].sendToAllClientsChannel(answer, 0, 0);
		if (all_cha.size() == 0)
			send_mess(him->getFd(), answer.c_str(), answer.length(), 0);
	}

}

void Server::part(std::vector<std::string> &cmd, int i)
{
	Client *her = &clients[i - 1];
	Channel* channel = getChannel(cmd[1]);
	std::string message = "PART";
	for (size_t j = 1; j < cmd.size(); j++)
		message += " " + cmd[j];
	std::string message_to_send = ":" + her->getNick() + "!" + her->getUser() + "@localhost" + " " + message + "\r\n";
	if (channel)
	{
		if (!delete_basics_client(her, channel, message_to_send))
			delete_operators_client(her, channel, message_to_send, i);
		her->removeChannel(channel->getName(), 1);
	}
	else
	{
		Client him = clients[i - 1];
		std::string error = ":" + him.getHost() + " 441 " + him.getNick() + " " + cmd[1] + " :Client is not on that channel;\r\n";
		send_mess(him.getFd(), error.c_str(), error.length(), 0);
	}
}

void Server::quit(std::vector<std::string> &cmd, int i)
{
	Client *him = &clients[i - 1];
	std::string message = "QUIT";
	for (size_t j = 1; j < cmd.size(); j++)
		message += " " + cmd[j];
	std::string message_to_send = ":" + him->getNick() + "!" + him->getUser() + "@localhost" + " " + message + "\r\n";
	std::vector<Channel> all_cha = him->getInChannels();
	for (size_t j = 0; j < all_cha.size(); ++j)
	{
		Channel *channel = getChannel(all_cha[j].getName());
		if (!delete_basics_client(him, channel, message_to_send))
			delete_operators_client(him, channel, message_to_send, i);
	}
	if (all_cha.size() == 0)
		send_mess(him->getFd(), message_to_send.c_str(), message_to_send.length(), 0);
	else
		delete_all_cha_client(him, all_cha);
	client_disconnected(i);
}

void Server::who(std::vector<std::string> &cmd, int i){
	Client *him = &clients[i - 1];
	Channel *channel = getChannel(cmd[1]);
	if (!channel){
		nonExistentChannel(cmd, i);
		return ;
	}
	std::string message = ":" + him->getHost() + " 352 " + him->getNick() + " " + cmd[1] + " ";
	std::string end = ":" + him->getHost() + " 315 " + him->getNick() + " " + cmd[1] + " :End of WHO list\r\n";
	std::vector<Client> _opes = channel->getArrayOperators();
	std::vector<Client> _basics = channel->getArrayBasicClient();
	for (size_t j = 0; j < _opes.size(); ++j){
		std::string to_send = message + _opes[j].getUser() + " localhost " + _opes[j].getHost() + " " + _opes[j].getNick() + " " + "H@ :0 " + _opes[j].getNick() + "\r\n";
		send(him->getFd(), to_send.c_str(), to_send.length(), 0);
	}
	for (size_t j = 0; j < _basics.size(); ++j){
		std::string to_send = message + _basics[j].getUser() + " localhost " + _basics[j].getHost() + " " + _basics[j].getNick() + " " + "H :0 " + _basics[j].getNick() + "\r\n";
		send(him->getFd(), to_send.c_str(), to_send.length(), 0);
	}
	send_mess(him->getFd(), end.c_str(), end.length(), 0);
}

void Server::handle_ping(std::vector<std::string> &cmd, int i) {
	std::string msg = join_cmd(cmd , " ", 1);
    std::string pong = "PONG " + msg + "\r\n";
	send_mess(clients[i - 1].getFd(), pong.c_str(), pong.length(), 0);
}