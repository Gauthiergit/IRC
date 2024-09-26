#include "Server.hpp"

int convert_port(std::string &port)
{
    for (size_t i = 0; i < port.length(); i++)
    {
        if (!isdigit(port[i]))
            throw(std::runtime_error("We need only digit for the port\n"));
    }
    double _port = std::atof(port.c_str());
    if (_port > 65535)
        throw(std::runtime_error("We need a value of port < 65535\n"));
    return (_port);
}

void verif_pwd(std::string &pwd)
{
	if (pwd.find(92))
		throw(std::runtime_error("Please don't select ascii 92 for the password\n"));
}

int main(int argc, char **argv)
{
    Server* server = NULL;

    try {
        if (argc != 3)
            throw std::runtime_error("Usage: program <port> <password>\n");
        std::string port_string(argv[1]);
		std::string pwd(argv[2]);
        int port = convert_port(port_string);
		// verif_pwd(pwd);
        signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
        server = new Server(port, argv[2]);
        server->init_serv();}
    catch (const std::exception& e) {
        if (server) {
            server->_free();
            delete server;
        }
        std::cerr << RED << e.what() << RESET;}

    return (0);
}