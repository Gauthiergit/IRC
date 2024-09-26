#include "Server.hpp"
#include "Channel.hpp"

/*---------------------------------------------------||||CHANNELS||||---------------------------------------------------*/

void	Server::addChannel(Channel newChannel){this->channels.push_back(newChannel);}

Channel*	Server::getChannel(std::string name)
{
	for (size_t i = 0; i < this->channels.size(); i++)
	{
		if (this->channels[i].getName() == name)
			return (&channels[i]);
	}
	return (NULL);
}

/*---------------------------------------------------||||DISCONNECTED||||---------------------------------------------------*/

void Server::client_disconnected(int i)
{
	std::cout << RED << "------------Client " << GREEN << clients[i - 1].getNick() << RED << " disconnected------------\n" << RESET << std::endl;

	if (this->fds[i].fd != -1)
    	close(this->fds[i].fd);
	for (int j = i; j < client_count; ++j)
    	fds[j] = fds[j + 1];
	if (i != client_count + 1 && this->fds[client_count].fd != -1)
		close(this->fds[client_count].fd);
    this->client_count--;
    clients[i - 1].clearBuffer();
    clients.erase(clients.begin() + i - 1);
}

/*---------------------------------------------------||||PARSING COMMAND||||---------------------------------------------------*/

std::vector<std::string>	Server::split_msg(std::string message)
{
	std::vector<std::string>	cmdTable;
	std::istringstream			iss(message);
	std::string					token;

	while (iss >> token)
	{
		cmdTable.push_back(token);
		token.clear();
	}
	return (cmdTable);
}

void Server::parsing_command(std::string &message, int i)
{
	typedef void (Server::*functionPointer)(std::vector<std::string> &, int);

	int len = message.find(" ");

    std::vector<std::string>	cmd = split_msg(message);
	// ------------------

	functionPointer functionTab[11] = {
		&Server::joinChannel,
		&Server::privmsg,
		&Server::kick,
		&Server::invite,
		&Server::topic,
		&Server::nick,
		&Server::parsing_mode,
		&Server::part,
		&Server::quit,
		&Server::who,
		&Server::handle_ping
	};
	std::string commands_maj[11] = {"JOIN", "PRIVMSG", "KICK", "INVITE", "TOPIC", "NICK", "MODE", "PART", "QUIT", "WHO", "PING"};
    std::string commands_min[11] = {"join", "privmsg", "kick", "invite", "topic", "nick", "mode", "part", "quit", "who", "ping"};

	int j = 0;
	while (j < 11){
		if (!message.compare(0, len, commands_maj[j]))
		{
			(this->*functionTab[j])(cmd, i);
			return;
        }
        else if (!message.compare(0, len, commands_min[j]))
		{
			(this->*functionTab[j])(cmd, i);
			return;
        }
		j++;
	}
}

void Server::parsing_mode(std::vector<std::string> &cmd, int i)
{
	typedef void (Server::*functionPointer)(std::vector<std::string> &, int, Channel*);

	if (cmd.size() < 2)
	{
		std::cerr<<RED<<"Error mode, you need at least : /MODE <channel> <option>"<<RESET<<std::endl;
		notEnoughParameters(cmd, i);
		return ;
	}
	Channel* channel = getChannel(cmd[1]);
	if (cmd.size() == 2)
	{
		if (!(channel)){
			std::cerr<<RED<<"Channel " << cmd[1] << " doesn't exist" << RESET << std::endl;
			nonExistentChannel(cmd, i);
			return ;
		}
		this->mode_options(i, channel);
		return ;
	}
	std::string option = cmd[2]; // extract option

	functionPointer functionTab[10] = {
		&Server::mode_plus_i,
		&Server::mode_minus_i,
		&Server::mode_plus_t,
		&Server::mode_minus_t,
		&Server::mode_plus_k,
		&Server::mode_minus_k,
		&Server::mode_plus_o,
		&Server::mode_minus_o,
		&Server::mode_plus_l,
		&Server::mode_minus_l
	};
	std::string commands[10] = {"+i", "-i", "+t", "-t", "+k", "-k", "+o", "-o", "+l", "-l"};

	int j = 0;
	while (j < 10){
		if (!option.compare(commands[j]))
		{
			if (!verif_channel_operator(cmd, i))
				return ;
			(this->*functionTab[j])(cmd, i, channel);
			return;
		}
		j++;
	}
}
