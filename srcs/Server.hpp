#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>

#include "ISocket.hpp"
#include "config.hpp"

class Server : public Socket {
 public:
  Server(config::Config config)
      : Socket(config.port, config.host), config_(config){};

  int SetSocket();

 private:
  config::Config config_;
};

#endif
