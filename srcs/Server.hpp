#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>

#include "Socket.hpp"

class Server : public Socket {
 public:
  Server(int port_, std::string host_ip__) : Socket(port_, host_ip__){};

  int SetSocket(int _fd);
  void SetStatus(int status) { socket_status = status; }
};

#endif
