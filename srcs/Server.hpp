#ifndef SERVER_HPP
#define SERVER_HPP

class Server;
#include "main.hpp"

class Server {
 public:
  Server(int port_, const char* host_) : port(port_), host(host_){};

  void init(void);
  long getFd(void) { return fd; };

  std::vector<long> client_fds;
  std::vector<long> writable_client_fds;

 private:
  long fd;
  struct sockaddr_in addr;
  int port;
  const char* host;
};

#endif
