#ifndef SERVER_HPP
#define SERVER_HPP

class Server;
#include "main.hpp"

class Server : public ISocket {
 public:
  Server(int port_, std::string hostIp_) : port(port_), hostIp(hostIp_){};

  long makeSocket(long _fd);

 private:
  struct sockaddr_in addr;
  int port;
  std::string hostIp;  // like "127.0.0.1"
};

#endif
