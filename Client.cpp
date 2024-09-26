#include "Client.hpp"

// CONSTRUCTOR AND DESTRUCTOR
Client::Client(int fd) : _fd(fd), _pwd(false), _auth(false), _user(""), _nick(""), _host("irc.example.com"), nb_mess(0), type_client(0){}
Client::~Client(){}

// GETTER
std::string const &Client::getUser(){return (this->_user);}
std::string const &Client::getNick(){return (this->_nick);}
std::string const &Client::getHost(){return (this->_host);}
bool &Client::getPwd(){return (this->_pwd);}
bool &Client::getAuth(){return (this->_auth);}
int &Client::getNb_mess(){return (this->nb_mess);}
int &Client::getFd(){return (this->_fd);}
int &Client::getTypeClient(){return (this->type_client);}
std::vector<Channel>& Client::getInChannels(){return (this->inChannels);}

// SETTER
void Client::setUser(std::string &user){this->_user = user;}
void Client::setNick(std::string &nick){this->_nick = nick;}
void Client::setHost(std::string &host){this->_host = host;}
void Client::setPwd(){this->_pwd = true;}
void Client::setAuth(){this->_auth = true;}
void Client::addNb_mess(){this -> nb_mess += 1;}

// METHODS
void Client::addChannel(Channel newChannel, int flag)
{
	if (!flag)
        inChannels.push_back(newChannel);
    else
        channels_im_invited.push_back(newChannel);
}

void Client::appendBuffer(const char* data, int size)
{
    buffer.append(data, size);
}

bool Client::hasCompleteMessage()
{
    size_t pos = buffer.find("\n");

	if (pos != std::string::npos && pos > 0 && type_client == 1 && buffer[pos - 1] == '\r'){
		return true;} // End of the message with \r\n
	if (pos != std::string::npos && type_client == 0){
        return true;} // End of the message with \n
    return false;
}

std::string Client::extractMessage()
{
    size_t pos = buffer.find("\n");
    std::string message;
	if (nb_mess < 2 && pos != std::string::npos && pos > 0 && buffer[pos - 1] == '\r'){
		type_client = 1;}
	if (type_client == 1){
    message = buffer.substr(0, pos - 1); // Extract without '\r' and '\n'
    buffer.erase(0, pos + 1); // delete message up to '\n'
	}
	else {
	message = buffer.substr(0, pos); // Extract without '\r' and '\n'
    buffer.erase(0, pos + 1); // delete message up to '\n'
	}
    return message;
}

void Client::clearBuffer()
{
    buffer.clear(); // Efface complètement le buffer
}

void	Client::removeChannel(std::string channel_name, int flag_in)
{
	if (!flag_in)
	{
		for (std::vector<Channel>::iterator it = channels_im_invited.begin(); it != channels_im_invited.end(); it++)
		{
			if (it->getName() == channel_name)
			{
				channels_im_invited.erase(it);
				return;
			}
		}
	}
	else
	{
		for (std::vector<Channel>::iterator it = inChannels.begin(); it != inChannels.end(); it++)
		{
			if (it->getName() == channel_name)
			{
				inChannels.erase(it);
				return;
			}
		}
	}
}

bool Client::findChannel(std::string channel_name)
{
    for (std::vector<Channel>::iterator it = channels_im_invited.begin(); it != channels_im_invited.end(); it++)
	{
		if (it->getName() == channel_name)
            return (true);
	}
    return (false);
}
