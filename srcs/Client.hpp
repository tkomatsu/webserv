#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "main.hpp"

enum SocketStatus {
  READ_CLIENT,
  READ_FILE,
  READ_CGI,
  WRITE_CGI,
  WRITE_FILE,
  WRITE_CLIENT,
};

class Client : public ISocket {
 public:
  long makeSocket(long _fd);
  void makeResponse(void);
  bool ParseRequest(void);
  void GenProcessForCGI(void);

  long recv(long client_fd);
  long send(long client_fd);

  static const int buf_max;

  int socket_status;

 private:
  std::string response;

  struct sockaddr_in addr;
  int port;
  std::string hostIp;
};

#endif
