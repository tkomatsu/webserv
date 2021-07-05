#ifndef SERVER_HPP
#define SERVER_HPP

class Server;
#include "main.hpp"

class Server : public ISocket {
 public:
  Server(int port_, const char* host_) : port(port_), host(host_){};

  long makeSocket(long _fd);

  std::vector<long> client_fds;

 private:
  struct sockaddr_in addr;
  int port;
  const char* host;
};

#endif
