#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ISocket.hpp"
#include "config.hpp"

class Server : public Socket {
 public:
  Server(const config::Config& config)
      : Socket(config.GetPort(), config.GetHost()), config_(config){};

  int OpenListenSocket();
  const config::Config& GetConfig() const;

 private:
  config::Config config_;
};

#endif
