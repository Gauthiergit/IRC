#include "Server.hpp"
#include "Channel.hpp"

void Server::authentication(std::string &message, int i){

	Client &client = clients[i - 1];

    switch (client.getNb_mess()) {
		case 1:
			if (clients[i - 1].getTypeClient() == 0)
				send_nc_message(i, "PASS");
			break;
        case 2:
            this->serv_check_pwd(message, i);
            break;
        case 3:
            this->serv_check_nick(message, i);
            break;
        case 4:
			this->serv_check_user(message, i);
            break;
        default:
            break;
    }

    if (client.getAuth()) {
        this->validate_auth(i);
    }
}

void Server::serv_check_pwd(std::string &message, int i) {
    if (message.substr(0, 5) != "PASS " || message.size() < 6) {
        err_pwd(i);
    } else if (message.substr(5) == this->_pwd) {
        clients[i - 1].setPwd();
		if (clients[i - 1].getTypeClient() == 0)
			send_nc_message(i, "NICK");
    } else {
        err_pwd(i);
    }
}

void Server::err_pwd(int i) {
    std::cerr << RED << "NO PASSWORD OR WRONG PASSWORD\n" << RESET;
    send_error_message(i, "464", "Password incorrect");
    client_disconnected(i);
}

void Server::serv_check_nick(std::string &message, int i) {
    if (message.substr(0, 5) != "NICK " || message.size() < 6) {
        err_nick(i);
    } else {
		std::string nick = message.substr(5);
		if (verif_nick(nick, i))
        	clients[i - 1].setNick(nick);
		else
			client_disconnected(i);
		if (clients[i - 1].getTypeClient() == 0)
			send_nc_message(i, "USER");
    }
}

void Server::err_nick(int i) {
    std::cerr << RED << "NO NICKNAME\n" << RESET;
    send_error_message(i, "431", "No nickname given");
    client_disconnected(i);
}

void Server::serv_check_user(std::string &message, int i) {
    if (message.substr(0, 5) != "USER " || message.size() < 6) {
        err_user(i);
    } else {
        std::istringstream iss(message);
        std::string user;
        iss >> user;
        iss >> user;
        clients[i - 1].setUser(user);
		clients[i - 1].setAuth();
    }
}

void Server::err_user(int i) {
    std::cerr << RED << "NO USER\n" << RESET;
    send_error_message(i, "461", "USER :Not enough parameters");
    client_disconnected(i);
}

void Server::send_nc_message(int i, const std::string &code) {
    std::string message = std::string(RED) + "THE NEXT COMMAND MUST SET THE " + code + ". IF NOT, YOU WILL BE KICKED\n" + std::string(RESET);
    send(this->fds[i].fd, message.c_str(), message.length(), 0);
}

void Server::send_error_message(int i, const std::string &code, const std::string &error_msg) {
    std::string message = ":" + clients[i - 1].getHost() + " " + code + " * :" + error_msg;
    send(this->fds[i].fd, message.c_str(), message.length(), 0);
}

void Server::validate_auth(int i){
	std::string welcomeMessage;
	std::string nick = clients[i - 1].getNick();
	std::string user = clients[i - 1].getUser();
	std::string host = clients[i - 1].getHost();

    std::cout << BLUE << "Server send to " << GREEN << clients[i - 1].getNick() << BLUE << " : " << std::endl;
    // 001 RPL_WELCOME
    welcomeMessage = ":" + host + " 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@client.host\r\n";
    send(this->fds[i].fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    std::cout << YELLOW << welcomeMessage;

    // 002 RPL_YOURHOST
    welcomeMessage = ":" + host + " 002 " + nick + " :Your host is irc.example.com, running version 1.0\r\n";
    send(this->fds[i].fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    std::cout << YELLOW << welcomeMessage;

    // 003 RPL_CREATED
    welcomeMessage = ":" + host + " 003 " + nick + " :This server was created Sat Sep 7 2024 at 15:00\r\n";
    send(this->fds[i].fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    std::cout << YELLOW << welcomeMessage;

    // 004 RPL_MYINFO
    welcomeMessage = ":" + host + " 004 " + nick + " irc.example.com 1.0 iowghraAsORTVSx NC\r\n";
    send(this->fds[i].fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    std::cout << YELLOW << welcomeMessage;

    // 375 RPL_MOTDSTART
    welcomeMessage = ":" + host + " 375 " + nick + " :- irc.example.com Message of the day -\r\n";
    send(this->fds[i].fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    std::cout << YELLOW << welcomeMessage;

    // 372 RPL_MOTD
    welcomeMessage = ":" + host + " 372 " + nick + " :- Welcome to the best IRC server!\r\n";
    send(this->fds[i].fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    std::cout << YELLOW << welcomeMessage;

    // 376 RPL_ENDOFMOTD
    welcomeMessage = ":" + host + " 376 " + nick + " :End of /MOTD command\r\n";
    send(this->fds[i].fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    std::cout << YELLOW << welcomeMessage << RESET << std::endl;
}
