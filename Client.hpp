#pragma once

#include <iostream>
#include "Channel.hpp"

class Channel;

class Client
{
    private :
		int			_fd;
		bool		_pwd;
		bool		_auth;
		std::string _user;
		std::string _nick;
		std::string _host;
		std::string buffer;
		std::vector<Channel> channels_im_invited;
		int			nb_mess;
		int			type_client;
		std::vector<Channel> inChannels;

    public :
    // CONSTRUCTOR AND DESTRUCTOR
		Client(int fd);
		~Client();

    // GETTER
		std::string const &getUser();
		std::string const &getNick();
		std::string const &getHost();
		bool	&getPwd();
		bool	&getAuth();
		int		&getNb_mess();
		int		&getFd();
		int		&getTypeClient();
		std::vector<Channel> &getInChannels();

	// SETTER
		void setUser(std::string &user);
		void setNick(std::string &nick);
		void setHost(std::string &host);
		void setPwd();
		void setAuth();
		void addNb_mess();

	// METHODS
		void addChannel(Channel newChannel, int flag);
		void appendBuffer(const char* data, int size);
		bool hasCompleteMessage();
		std::string extractMessage();
		void clearBuffer();
		void removeChannel(std::string channel_name, int flag_in);
		bool findChannel(std::string channel_name);
};