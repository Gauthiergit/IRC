#include "Server.hpp"
#include "Channel.hpp"

/*---------------------------------------------------||||TOOLS||||---------------------------------------------------*/

bool Server::verif_channel_operator(std::vector<std::string> &cmd, int i){
	Channel *_channel = this->getChannel(cmd[1]);
	if (cmd[1] == clients[i - 1].getNick())
	{
		std::string msg = ":" + cmd[1] + " MODE " + cmd[1] + " +i\r\n";
		send_mess(clients[i - 1].getFd(), msg.c_str(), msg.length(), 0);
		return false;
	}
	if (!(_channel)){
		std::cerr<<RED<<"Channel " << cmd[1] << " doesn't exist" << RESET << std::endl;
		nonExistentChannel(cmd, i);
		return false;
	}
	Client *_operator = _channel->getOperator(clients[i - 1].getNick());
	if (!(_operator)){
		std::cerr<<RED << clients[i - 1].getNick() << " is not an operator" << RESET << std::endl;
		notChannelOperator(cmd, i);
		return false;
	}
	return true;
}

bool Server::verif_nick(std::string new_nick, int i){
	Client *him = &clients[i - 1];
	for (int j = 0; j < this->client_count - 1; ++j){
		if (clients[j].getNick() == new_nick){
			std::string error = ":" + him->getHost() + " 433 " + him->getNick() + " " + new_nick + " :Nickname is already in use\r\n";
			send_mess(him->getFd(), error.c_str(), error.length(), 0);
			return false;
		}
	}
	return true;
}

bool Server::verif_client_in_channel(std::string channel_name, int i)
{
	std::vector<Channel> client_channels = clients[i - 1].getInChannels();
	for (size_t j = 0; j < client_channels.size(); j++)
	{
		if (client_channels[j].getName() == channel_name)
			return (true);
	}
	return (false);
}

std::string size_t_to_string(size_t value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

bool	Server::client_exist(std::string client_nick)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].getNick() == client_nick)
			return (true);
	}
	return (false);
}

bool Server::delete_basics_client(Client *him, Channel *cha, std::string &message){
	for (size_t i = 0; i < cha->getArrayBasicClient().size(); i++){
		if (cha->getArrayBasicClient()[i].getNick() == him->getNick())
		{
			cha->sendToAllClientsChannel(message, 0, 0);
			cha->removeBasicClient(him->getNick());
			him->removeChannel(cha->getName(), 1);
			return true;
		}
	}
	return false;
}

void Server::delete_operators_client(Client *him, Channel *cha, std::string &message, int i){
	for (size_t j = 0; j < cha->getArrayOperators().size(); j++){
		if (cha->getArrayOperators()[j].getNick() == him->getNick())
		{
			mode_before_leave(cha, i);
			cha->sendToAllClientsChannel(message, 0, 0);
			cha->removeOperator(him->getNick());
			him->removeChannel(cha->getName(), 1);
		}
	}
}

void Server::mode_before_leave(Channel *cha, int i){
	std::vector<std::string> cmd;
	cmd.push_back("MODE");
	cmd.push_back(cha->getName());
	if (cha->getArrayOperators().size() < 2 && cha->getArrayBasicClient().size() > 0)
	{
		cmd.push_back("+o");
		cmd.push_back(cha->getArrayBasicClient()[0].getNick());
		mode_plus_o(cmd, i, cha);
	}
}

void Server::delete_all_cha_client(Client *him, std::vector<Channel> all_cha){
	size_t i = 0;
	while (i < all_cha.size()) {
        him->removeChannel(all_cha[i].getName(), 1);
        all_cha.erase(all_cha.begin() + i);
    }
}
bool	Server::client_already_in_channel(std::string client_nick, std::string channel_name)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].getNick() == client_nick)
		{
			std::vector<Channel> client_channels = clients[i].getInChannels();
			for (size_t j = 0; j < client_channels.size(); j++)
			{
				if (client_channels[j].getName() == channel_name)
					return (true);
			}
		}
	}
	return (false);
}

Client*	Server::getClient(std::string client_nick)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].getNick() == client_nick)
			return (&clients[i]);
	}
	return (NULL);
}

std::string to_string(long long value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void send_mess(int fd, const char *msg, size_t length, int i){
	send(fd, msg, length, i);
	std::cout << BLUE << "Server send : \n" << YELLOW << msg << std::endl;
}

/*---------------------------------------------------||||ERRORS||||---------------------------------------------------*/

void Server::notChannelOperator(std::vector<std::string> &cmd, int i)
{
	std::string error = ":server 482 " + clients[i - 1].getNick() + " " + cmd[1] + " :You're not channel operator.\r\n";
	if (cmd[0] == "INVITE" || cmd[0] == "invite")
		error = ":server 482 " + clients[i - 1].getNick() + " " + cmd[2] + " :You're not channel operator.\r\n";
	send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
}

void Server::notEnoughParameters(std::vector<std::string> &cmd, int i)
{
	std::string error = ":server 461 " + clients[i - 1].getNick() + " " + cmd[0] + " :Not enough parameters\r\n";
	send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
}

void Server::nonExistentChannel(std::vector<std::string> &cmd, int i)
{
	std::string error = ":server 403 " + clients[i - 1].getNick() + " " + cmd[1] + " :No such channel\r\n";
	if (cmd[0] == "INVITE" || cmd[0] == "invite")
		error = ":server 403 " + clients[i - 1].getNick() + " " + cmd[2] + " :No such channel\r\n";
	send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
}

void	Server::not_in_channel(std::vector<std::string> &cmd, int i){
	Client him = clients[i - 1];
	std::string error = ":" + him.getHost() + " 441 " + him.getNick() + " " + cmd[2] + " " + cmd[1] + " :Client is not on that channel;\r\n";
	send_mess(him.getFd(), error.c_str(), error.length(), 0);
}

void	Server::in_channel(std::vector<std::string> &cmd, int i){
	Client him = clients[i - 1];
	std::string error = ":" + him.getHost() + " 443 " + him.getNick() + " " + cmd[1] + " " + cmd[2] + " :Client already in that channel;\r\n";
	send_mess(him.getFd(), error.c_str(), error.length(), 0);
}

void 	Server::client_doesnt_exist(std::vector<std::string> &cmd, int i)
{
	std::string error = ":" + clients[i - 1].getHost() +  " 401 " + clients[i - 1].getNick() + " " + cmd[2] + "  :No such nick/\r\n";
	if (cmd[0] == "INVITE" || cmd[0] == "invite")
		error = ":" + clients[i - 1].getHost() +  " 401 " + clients[i - 1].getNick() + " " + cmd[1] + "  :No such nick/\r\n";
	send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
}