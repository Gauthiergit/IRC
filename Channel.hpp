#pragma once
#include "Server.hpp"
#include "Client.hpp"

class Client;

class Channel
{
    private:
        bool			_invite;
        std::string		_key;
        bool            _keyAccess;
        bool			_limit;
        bool		    _topicAccess;
		std::time_t		_topic_timestamp;
		std::string		_ope_who_change_topic;
        std::string	    _name;
        std::string		_topic;
        std::vector<Client> basicClients;
        std::vector<Client> operators;
		size_t			_limit_value;

    public:
        Channel(std::string const &name);
        ~Channel();

        // -----Getters
        bool			getInvite();
        bool			getTopicAccess();
        std::string		getKey();
        bool			getLimit();
		bool	        getKeyAccess();
        std::string		getName();
		size_t			getLimitValue();
		std::string const &getTopic();
		std::time_t		getTopicTimestamp();
		std::string const &getOpeWhoChangeTopic();
        std::vector<Client>& getArrayBasicClient();
        std::vector<Client>& getArrayOperators();
		Client			*getBasicClient(std::string name);
		Client			*getOperator(std::string name);
        std::string		getListBasicClients();
        std::string		getListOperators();

		// -----Setters
		void	setInvite(bool invite);
		void	setTopicAccess(bool topic);
		void	setKey(std::string key);
        void	setLimit(bool limit);
        void	setName(std::string name);
		void	setKeyAccess(bool key);
		void 	setLimitValue(size_t limit);
		void	setTopic(std::string topic);
		void 	setTopicTimestamp();
		void	setOpeWhoChangeTopic(std::string ope_nick);

        // -----Methods
        void    addBasicClient(Client newClient);
        void    addOperator(Client newClient);
		void	removeBasicClient(std::string nick);
		void	removeOperator(std::string nick);
		bool	changeRights_BasicClientToOpe(std::string nick);
		bool	changeRights_OpeToBasicClient(std::string nick);
        size_t  countOperators();
        size_t  countBasicClients();
        void    sendToAllClientsChannel(std::string& msg, int client_fd, int flag);
};