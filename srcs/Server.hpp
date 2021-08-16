#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>

#include "Socket.hpp"
#include "config.hpp"

class Server : public Socket {
 public:
  Server(config::Config config) : Socket(config.port, config.host), config_(config) {};
  int GetStatus() { return socket_status_; };
 private:
  config::Config config_;
};

#endif
