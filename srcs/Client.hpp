#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "main.hpp"

class Client : public ISocket {
 public:
  long makeSocket(long _fd);
  void makeResponse(void);

  long recv(long client_fd);
  long send(long client_fd);

  static const int buf_max;

 private:
  std::string response;

  struct sockaddr_in addr;
  int port;
  std::string hostIp;
};

#endif
