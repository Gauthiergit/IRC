#include "Channel.hpp"

// -----Constructor and Destructor
Channel::Channel(std::string const &name) : _invite(false), _key(""), _keyAccess(false), _limit(false), _topicAccess(false), _topic_timestamp(0), _ope_who_change_topic(""), _name(name), _topic(""){}
Channel::~Channel(){}

// -----Getters
bool		Channel::getInvite(){return (this->_invite);}
bool		Channel::getTopicAccess(){return (this->_topicAccess);}
std::string	Channel::getKey(){return (this->_key);}
bool		Channel::getLimit(){return (this->_limit);}
bool		Channel::getKeyAccess(){return (this->_keyAccess);}
std::string	Channel::getName(){return (this->_name);}
size_t 		Channel::getLimitValue(){return (this->_limit_value);}
std::string const &Channel::getTopic(){return (this->_topic);}
std::time_t	Channel::getTopicTimestamp(){return (this->_topic_timestamp);}
std::string const &Channel::getOpeWhoChangeTopic(){return (this->_ope_who_change_topic);}
std::vector<Client>& Channel::getArrayBasicClient(){return (this->basicClients);}
std::vector<Client>& Channel::getArrayOperators(){return (this->operators);}

Client*	Channel::getBasicClient(std::string name)
{
	for (size_t i = 0; i < this->basicClients.size(); i++)
	{
		if (this->basicClients[i].getNick() == name)
			return (&basicClients[i]);
	}
	return (NULL);
}

Client*	Channel::getOperator(std::string name)
{
	for (size_t i = 0; i < this->operators.size(); i++)
	{
		if (this->operators[i].getNick() == name)
			return (&operators[i]);
	}
	return (NULL);
}

std::string Channel::getListBasicClients()
{
	std::string res;
	for (size_t j = 0; j < basicClients.size(); j++)
	{
		if (j != 0)
			res += " ";
		res += basicClients[j].getNick();
	}
	return (res);
}

std::string Channel::getListOperators()
{
	std::string res;
	for (size_t j = 0; j < operators.size(); j++)
	{
		if (j != 0)
			res += " ";
		res += "@" + operators[j].getNick();
	}
	return (res);
}

// -----Setters
void	Channel::setInvite(bool invite){this->_invite = invite;}
void	Channel::setTopicAccess(bool topic){this->_topicAccess = topic;}
void	Channel::setKey(std::string key){this->_key = key;}
void	Channel::setLimit(bool limit){this->_limit = limit;}
void	Channel::setName(std::string name){this->_name = name;}
void	Channel::setKeyAccess(bool key){this->_keyAccess = key;}
void    Channel::setLimitValue(size_t limit) {this->_limit_value = limit;}
void	Channel::setTopic(std::string topic){this->_topic = topic;}

void	Channel::setTopicTimestamp()
{
	std::time_t current_time = std::time(0);
	this->_topic_timestamp = current_time;
}

void	Channel::setOpeWhoChangeTopic(std::string ope_nick){this->_ope_who_change_topic = ope_nick;}


// -----Methods
void	Channel::addBasicClient(Client newClient){basicClients.push_back(newClient);}
void    Channel::addOperator(Client newClient){operators.push_back(newClient);}

void	Channel::removeBasicClient(std::string nick)
{
	for (std::vector<Client>::iterator it = this->basicClients.begin(); it != this->basicClients.end(); it++)
	{
		if (it->getNick() == nick)
		{
			this->basicClients.erase(it);
			return;
		}
	}
}

void	Channel::removeOperator(std::string nick)
{
	for (std::vector<Client>::iterator it = this->operators.begin(); it != this->operators.end(); it++)
	{
		if (it->getNick() == nick)
		{
			this->operators.erase(it);
			return;
		}
	}
}

bool	Channel::changeRights_BasicClientToOpe(std::string nick)
{
	for (size_t i = 0; i < basicClients.size(); i++)
	{
		if (basicClients[i].getNick() == nick)
		{
			operators.push_back(basicClients[i]);
			basicClients.erase(basicClients.begin() + i);
			return (true);
		}
	}
	return (false);
}

bool	Channel::changeRights_OpeToBasicClient(std::string nick)
{
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (operators[i].getNick() == nick)
		{
			basicClients.push_back(operators[i]);
			operators.erase(operators.begin() + i);
			return (true);
		}
	}
	return (false);
}

size_t     Channel::countOperators()
{
	return (this->operators.size());
}

size_t     Channel::countBasicClients()
{
	return (this->basicClients.size());
}

void    Channel::sendToAllClientsChannel(std::string& msg, int client_fd, int flag)
{
	if (!flag)
	{
		for (size_t j = 0; j < operators.size(); j++)
			send(operators[j].getFd(), msg.c_str(), msg.length(), 0);
		for (size_t j = 0; j < basicClients.size(); j++)
			send(basicClients[j].getFd(), msg.c_str(), msg.length(), 0);
	}
	else
	{
		for (size_t j = 0; j < basicClients.size(); j++)
        {
            if (basicClients[j].getFd() != client_fd)
                send(basicClients[j].getFd(), msg.c_str(), msg.length(), 0);
        }
        for (size_t j = 0; j < operators.size(); j++)
        {
            if (operators[j].getFd() != client_fd)
                send(operators[j].getFd(), msg.c_str(), msg.length(), 0);
        }
	}
	std::cout << BLUE << "Server send to all Clients in Channel " << GREEN << this->getName() << "\n" << YELLOW << msg << std::endl;
}
