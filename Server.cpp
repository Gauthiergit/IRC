#include "Server.hpp"

bool Server::Signal = false;
// CONSTRUCTOR AND DESTRUCTOR
Server::Server(const int &port, const std::string &pwd) : _port(port), _pwd(pwd), client_count(0){
    this->server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    fcntl(this->server_fd, F_SETFL, O_NONBLOCK);// socket in not blocked mode
    if (server_fd == -1)
        throw(std::runtime_error("Error, impossible to create the socket\n"));
    this->address.sin_family = AF_INET;            // IPv4
    this->address.sin_addr.s_addr = INADDR_ANY;    // Configures the socket to listen on all IP addresses available on the host.
    this->address.sin_port = htons(this->_port);   // Use port

    for (int i = 0; i < MAX_CLIENTS; i++) {
        this->fds[i].fd = -1;
        this->fds[i].events = 0;
        this->fds[i].revents = 0;
    }
}

Server::~Server(){}

// METHODS
void Server::init_serv(){
    /*bind(server_fd, (struct sockaddr*)&address, sizeof(address)) :
    The bind() function is used to bind a socket to a specific IP address and port number.*/
    if (bind(this->server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
        throw(std::runtime_error("Error when binding\n"));

    // Prepare to accept connections on socket
    if (listen(this->server_fd, MAX_CLIENTS) < 0)
        throw(std::runtime_error("Error when listening\n"));

    //  Definition of struct pollfd
    /*struct pollfd is used by poll() to monitor file descriptors. Each entry in the fds array
    represents a FD you want to monitor.*/

    this->fds[0].fd = this->server_fd; /*The first FD is the server FD, it uses to accept new connections*/
    this->fds[0].events = POLLIN; /*POLLIN eventmeans you want to be notified when data is available
    for reading on this descriptor (for example, when a new connection arrives on the server socket).*/
    this->poll_events();
}

void Server::poll_events(){
    this->client_count = 1;
    while (Server::Signal == false){
        int poll_count = poll(this->fds, this->client_count, -1);
        if (Server::Signal == true)
            throw(std::runtime_error("\nSignal received\n"));
        if (poll_count < 0)
            throw(std::runtime_error("Error when polling\n"));
        for (int i = 0; i < this->client_count; i++){
            if (this->fds[i].revents & POLLIN){
                if (i == 0)
                    this->new_connection();
                else
                    this->serv_rcvd(i);
            }
        }
    }
    throw(std::runtime_error("Signal received\n"));
}

void Server::new_connection()
{
    int new_socket = accept(this->server_fd, NULL, NULL);
    fcntl(new_socket, F_SETFL, O_NONBLOCK); // socket in not blocked mode
    if (new_socket < 0)
        std::cerr << "Error accepting connection" << std::endl;
    else {
        if (this->client_count < MAX_CLIENTS) { /*Verify the number of clients*/
            this->fds[this->client_count].fd = new_socket; /*Add the fd new_socket at the list to survey*/
            this->fds[this->client_count].events = POLLIN;
            this->client_count++;
            clients.push_back(Client(new_socket));
            std::cout << GREEN << "------------New connection accepted------------\n" << RESET << std::endl;
            std::string message = std::string(GREEN) + "------New socket Added to server------\n" + std::string(MAGENTA) + "Please send a first message to identify the type of customer\n" + RESET;
            send (new_socket, message.c_str(), message.size(), 0);
            }
        else {
            std::cerr << "Maximum number of clients reached" << std::endl;
            close(new_socket);}}
}

void Server::serv_rcvd(int i){
    char buffer[1024] = {0};
    int bytes_received = recv(this->fds[i].fd, buffer, 1024, 0);
    if (bytes_received <= 0)
		this->client_disconnected(i);
    else {
        clients[i - 1].appendBuffer(buffer, bytes_received); // Append the data to the client's buffer
        while (clients[i - 1].hasCompleteMessage()) {
            clients[i - 1].addNb_mess();
            std::string message = clients[i - 1].extractMessage(); // Extract a complete message
            std::cout << CYAN << "Message received from " << GREEN << clients[i - 1].getNick() << CYAN << " : \n" << MAGENTA << message << "\n" << RESET << std::endl;
			if (clients[i - 1].getNb_mess() < 5)
				this->authentication(message, i); //Validate or not the Auth
			else if (clients[i - 1].getAuth() && clients[i - 1].getNb_mess() > 4)
				this->parsing_command(message, i);
        }
    }
}

void Server::_free()
{
    for(int i = 0; i < this->client_count; i++){
        if (this->fds[i].fd != -1)
            close(this->fds[i].fd);
    }
    if (this->server_fd > -1)
        close(this->server_fd);
}

void Server::SignalHandler(int signum)
{
	(void)signum;
	Server::Signal = true;
}
