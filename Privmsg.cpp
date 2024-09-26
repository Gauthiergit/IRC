#include "Server.hpp"
#include "Channel.hpp"

std::string join_cmd(const std::vector<std::string>& elements, const std::string& delimiter, size_t index)
{
    std::ostringstream oss;
    for (size_t i = index; i < elements.size(); ++i) {
        if (i > index)
            oss << delimiter;
        oss << elements[i];
    }
    return oss.str();
}

void    Server::privmsg(std::vector<std::string> &cmd, int i)
{
	if (cmd.size() < 3)
	{
		notEnoughParameters(cmd, i);
		return;
	}
    std::string msg = join_cmd(cmd, " ", 2);
    std::string answer = ":" + this->clients[i - 1].getNick() + " " + cmd[0] + " " + cmd[1] + " " + msg + "\r\n";
    Channel* channel = getChannel(cmd[1]);
	if (channel != NULL)
	{
		if (verif_client_in_channel(channel->getName(), i))
		{
			channel->sendToAllClientsChannel(answer, clients[i - 1].getFd(), 1);
			return;
		}
		std::string error = ":" + clients[i - 1].getHost() + " 404 " + clients[i - 1].getNick() + " " + cmd[1] + " :Cannot send to channel\r\n";
		send_mess(clients[i - 1].getFd(), error.c_str(), error.length(), 0);
	}
    else
    {
        for (size_t j = 0; j < clients.size(); j++)
        {
            if (clients[j].getNick() == cmd[1])
            {
                answer = ":" + clients[i - 1].getNick() + "!" + clients[i - 1].getUser() + "@" +  clients[i - 1].getHost() + " " + cmd[0] + " " + cmd[1] + " " + msg + "\r\n";
                send_mess(clients[j].getFd(), answer.c_str(), answer.length(), 0);
                return;
            }
        }
		client_doesnt_exist(cmd, i);
    }
}
