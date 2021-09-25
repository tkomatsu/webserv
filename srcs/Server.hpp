#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "ISocket.hpp"
#include "config.hpp"

class Server : public ISocket {
 public:
  Server(const config::Config& config);
  ~Server();

  int OpenListenSocket();
  std::map<std::string, config::Config>& GetConfig();

 private:
  /* prohibit copy constructor and assignment operator */
  Server(const Server&);
  Server& operator=(const Server&);

  std::map<std::string, config::Config> config_;
};

#endif /* SERVER_HPP */
