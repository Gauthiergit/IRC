#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
#include <pthread.h>

#include <sstream>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <sstream>  // Pour std::ostringstream
#include "Client.hpp"
#include "Channel.hpp"
// #include "replies.hpp"

# define RESET "\033[0m"
# define BLUE "\033[94m"
# define RED "\033[91m"
# define GREEN "\033[92m"
# define YELLOW "\033[33m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"

#define MAX_CLIENTS 10

class Channel;

class Client;

class Server
{
    private :
		const int _port;
		std::string const _pwd;
		int server_fd;
		struct sockaddr_in address;
		struct pollfd fds[MAX_CLIENTS];
		static bool Signal;
		int client_count;
		std::vector<Client> clients;
		std::vector<Channel> channels;
		std::vector<pthread_t> threads;

    public :
    // CONSTRUCTOR AND DESTRUCTOR
		Server(const int &port, const std::string &pwd);
		~Server();

    // METHODS

	/*-------Serveur initialisation, poll_events, connection, free all-------*/
	// Server.cpp:
		void	init_serv();
		void	poll_events();
		void	new_connection();
		void	serv_rcvd(int i);
		void	_free();
    	static void	SignalHandler(int signum);

	/*-------Authentication-------*/
	// Authentication.cpp:
		void	authentication(std::string &message, int i);
		void	serv_check_pwd(std::string &message, int i);
		void	err_pwd(int i);
		void	serv_check_nick(std::string &message, int i);
		void	err_nick(int i);
		void	serv_check_user(std::string &message, int i);
		void	err_user(int i);
		void	send_nc_message(int i, const std::string &nessage);
		void	send_error_message(int i, const std::string &code, const std::string &error_msg);
		void	validate_auth(int i);

	/* -----Client----------- */
	Client *getClient(std::string client_nick);

	/*-------Channel-------*/
		void		addChannel(Channel newChannel);
		Channel*	getChannel(std::string name);
    	// void		removeChannel(std::string name);

	/*-------Disconnected-------*/
		void client_disconnected(int i);

    /*-------Parsing commands-------*/
		std::vector<std::string>	split_msg(std::string message);
		void parsing_command(std::string &message, int i);
		void parsing_mode(std::vector<std::string> &cmd, int i);

    /*-------Operator commands-------*/
		void	joinChannel(std::vector<std::string> &cmd, int i);
		void    createChannel(std::string channelName, int i);
		void    join(std::string channelName, std::string key, int i);
		void	quitAllChannels(Client her);
		void    privmsg(std::vector<std::string> &cmd, int i);
		void    kick(std::vector<std::string> &cmd, int i);
		void    invite(std::vector<std::string> &cmd, int i);
		void    topic(std::vector<std::string> &cmd, int i);
		void	part(std::vector<std::string> &cmd, int i);
		void    quit(std::vector<std::string> &cmd, int i);
		void 	handle_ping(std::vector<std::string> &cmd, int i);
		void    mode_plus_i(std::vector<std::string> &cmd, int i, Channel* channel);
		void    mode_minus_i(std::vector<std::string> &cmd, int i, Channel* channel);
		void    nick(std::vector<std::string> &cmd, int i);
		void    mode_plus_t(std::vector<std::string> &cmd, int i, Channel* channel);
		void    mode_minus_t(std::vector<std::string> &cmd, int i, Channel* channel);
		void    mode_plus_k(std::vector<std::string> &cmd, int i, Channel* channel);
		void    mode_minus_k(std::vector<std::string> &cmd, int i, Channel* channel);
		void    mode_plus_o(std::vector<std::string> &cmd, int i, Channel* channel);
		void    mode_minus_o(std::vector<std::string> &cmd, int i, Channel* channel);
		void    mode_plus_l(std::vector<std::string> &cmd, int i, Channel *channel);
		void    mode_minus_l(std::vector<std::string> &cmd, int i, Channel *channel);
		void	mode_options(int i, Channel *channel);
		void	who(std::vector<std::string> &cmd, int i);
		void	ping(std::vector<std::string> &cmd, int i);

	/*-------Tools-------*/
		bool 	verif_channel_operator(std::vector<std::string> &cmd, int i);
		bool    verif_nick(std::string new_nick, int i);
		bool 	verif_client_in_channel(std::string channel_name, int i);
		bool	delete_basics_client(Client *him, Channel *cha, std::string &message);
		void	delete_operators_client(Client *him, Channel *cha, std::string &message, int i);
		void	mode_before_leave(Channel *cha, int i);
		void	delete_all_cha_client(Client *him, std::vector<Channel> all_cha);
		bool	client_exist(std::string client_nick);
		bool	client_already_in_channel(std::string client_nick, std::string channel_name);

	/*-------Errors-------*/
		void 	notEnoughParameters(std::vector<std::string> &cmd, int i);
		void 	nonExistentChannel(std::vector<std::string> &cmd, int i);
		void 	notChannelOperator(std::vector<std::string> &cmd, int i);
		void	not_in_channel(std::vector<std::string> &cmd, int i);
		void	in_channel(std::vector<std::string> &cmd, int i);
		void 	client_doesnt_exist(std::vector<std::string> &cmd, int i);
};

std::string join_cmd(const std::vector<std::string>& elements, const std::string& delimiter, size_t index);
std::string size_t_to_string(size_t value);
std::string to_string(long long value);
void 		send_mess(int fd, const char *msg, size_t length, int i);
#endif
