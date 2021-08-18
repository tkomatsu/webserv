#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>

#include "ISocket.hpp"
#include "config.hpp"

class Server : public Socket {
 public:
/* <<<<<<< HEAD
  Server(int port, std::string host_ip) : Socket(port, host_ip){};
  int GetPort(void);
  std::string GetHost(void);
======= */
  Server(config::Config config)
      : Socket(config.port, config.host), config_(config){};

  int SetSocket(void);
  const config::Config GetConfig();

 private:
  config::Config config_;
};

#endif
