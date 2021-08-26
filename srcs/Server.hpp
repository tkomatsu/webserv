#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>

#include "ISocket.hpp"
#include "config.hpp"

class Server : public Socket {
 public:
  Server(const config::Config& config)
      : Socket(config.GetPort(), config.GetHost()), config_(config){};

  int SetSocket();
  const config::Config& GetConfig() const;

 private:
  config::Config config_;
};

#endif
