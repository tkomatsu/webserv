#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>

#include "Socket.hpp"

class Server : public Socket {
 public:
  Server(int port, std::string host_ip) : Socket(port, host_ip){};
  int GetStatus() { return socket_status_; };
};

#endif
