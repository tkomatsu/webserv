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

class Server : public ISocket {
 public:
  Server(const config::Config& config);
  ~Server();

  int OpenListenSocket();
  const config::Config& GetConfig() const;

 private:
  Server(const Server&);
  Server& operator=(const Server&);

  config::Config config_;
};

#endif
