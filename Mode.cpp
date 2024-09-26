#include "Server.hpp"

void Server::mode_options(int i, Channel *channel){
	Client him = clients[i - 1];
	std::string begin = ":" + him.getHost() + " 324 " + him.getNick() + " " + channel->getName() + " ";
	std::string mode_string = "+";
	std::string additional_params;

	// Ajouter dynamiquement les modes en fonction des booleens
	if (channel->getInvite()) {
		mode_string += "i";
	}
	if (channel->getTopicAccess()) {
		mode_string += "t";
	}
	if (channel->getLimit()) {
		mode_string += "l";
		additional_params += " " + size_t_to_string(channel->getLimitValue());  // Ajout de la limite d'utilisateurs
	}
	if (channel->getKeyAccess()) {
		mode_string += "k";
		additional_params += " " + channel->getKey();  // Ajout du mot de passe
	}
	if (mode_string == "+")
		mode_string = ":";
	std::string response = begin + mode_string + additional_params;

	// Envoyer le message au client
	send_mess(fds[i].fd, response.c_str(), response.length(), 0);
}

void Server::mode_plus_i(std::vector<std::string> &cmd, int i, Channel* channel){
	channel->setInvite(true);
	std::string msg = ":" + clients[i - 1].getNick() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + "\r\n";
	channel->sendToAllClientsChannel(msg, 0, 0);
}

void Server::mode_minus_i(std::vector<std::string> &cmd, int i, Channel* channel){
	channel->setInvite(false);
	std::string msg = ":" + clients[i - 1].getNick() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + "\r\n";
	channel->sendToAllClientsChannel(msg, 0, 0);
}

void Server::mode_plus_t(std::vector<std::string> &cmd, int i, Channel* channel){
	channel->setTopicAccess(true);
	std::string msg = ":" + clients[i - 1].getNick() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + "\r\n";
	channel->sendToAllClientsChannel(msg, 0, 0);
}

void Server::mode_minus_t(std::vector<std::string> &cmd, int i, Channel* channel){
	channel->setTopicAccess(false);
	std::string msg = ":" + clients[i - 1].getNick() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + "\r\n";
	channel->sendToAllClientsChannel(msg, 0, 0);
}

void Server::mode_plus_k(std::vector<std::string> &cmd, int i, Channel* channel){
	(void)i;
	if (cmd.size() < 4){
		std::cerr << RED << "No key in params for mode +k" << RESET << std::endl;
		notEnoughParameters(cmd, i);
		return ;
	}
	else
	{
		std::cout << MAGENTA << "The key will be : " << cmd[3] << RESET << std::endl;
		channel->setKey(cmd[3]);
		channel->setKeyAccess(true);
		std::string msg = ":" + clients[i - 1].getHost() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + " " + cmd[3] + "\r\n";
		channel->sendToAllClientsChannel(msg, 0, 0);
	}
}

void Server::mode_minus_k(std::vector<std::string> &cmd, int i, Channel* channel){
	(void)i;
	channel->setKey("");
	channel->setKeyAccess(false);
	std::string msg = ":" + clients[i - 1].getHost() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + " " + cmd[3] + "\r\n";
	channel->sendToAllClientsChannel(msg, 0, 0);
}

void Server::mode_plus_o(std::vector<std::string> &cmd, int i, Channel* channel){
	if (cmd.size() < 4){
		std::cerr << RED << "No client name in params for mode +o" << RESET << std::endl;
		notEnoughParameters(cmd, i);
		return ;
	}
	Client *_client = channel->getBasicClient(cmd[3]);
	if (_client != NULL)
	{
		channel->changeRights_BasicClientToOpe(_client->getNick());
		std::string msg = ":" + clients[i - 1].getNick() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + " " + cmd[3] + "\r\n";
		channel->sendToAllClientsChannel(msg, 0, 0);
	}
	else
		client_doesnt_exist(cmd, i);
}

void Server::mode_minus_o(std::vector<std::string> &cmd, int i, Channel* channel){
	if (cmd.size() < 4){
		std::cerr << RED << "No client name in params for mode -o" << RESET << std::endl;
		notEnoughParameters(cmd, i);
		return ;
	}
	Client *_client = channel->getOperator(cmd[3]);
	if (_client != NULL)
	{
		if ((int)getChannel(cmd[1])->countOperators() == 1
			&& cmd[3] == clients[i - 1].getNick()){
			std::cerr << RED << "at least 1 operator is required" << RESET << std::endl;
			std::string error = ":" + clients[i - 1].getHost() + " 482 " + clients[i - 1].getNick() + " " + cmd[1] + " :Cannot remove operator privileges, you're the last operator";
			send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
		}
		else
		{
			channel->changeRights_OpeToBasicClient(_client->getNick());
			std::string msg = ":" + clients[i - 1].getNick() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + " " + cmd[3] + "\r\n";
			channel->sendToAllClientsChannel(msg, 0, 0);
		}
	}
	else
		client_doesnt_exist(cmd, i);
}

void Server::mode_plus_l(std::vector<std::string> &cmd, int i, Channel *channel){
	(void)i;
	if (cmd.size() < 4){
		std::cerr << RED << "No limit set for +l" << RESET << std::endl;
		notEnoughParameters(cmd, i);
		return ;
	}
	size_t limits = channel->countOperators() + channel->countBasicClients();
	if (limits > std::atof(cmd[3].c_str())){
		std::cerr << RED << "Error limit must be > " << limits << " (numbers of ALL clients)" << RESET << std::endl;
		return ;}
	channel->setLimitValue(std::atof(cmd[3].c_str()));
	channel->setLimit(true);
	std::string msg = ":" + clients[i - 1].getHost() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + " " + cmd[3] + "\r\n";
	channel->sendToAllClientsChannel(msg, 0, 0);
}

void Server::mode_minus_l(std::vector<std::string> &cmd, int i, Channel *channel){
	(void)i;
	channel->setLimit(false);
	std::string msg = ":" + clients[i - 1].getHost() + " " + cmd[0] + " " + cmd[1] + " " + cmd[2] + "\r\n";
	channel->sendToAllClientsChannel(msg, 0, 0);
}
