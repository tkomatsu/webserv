#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

#include <string>

enum SocketStatus {
  READ_CLIENT,
  READ_FILE,
  READ_CGI,
  WRITE_CGI,
  WRITE_FILE,
  WRITE_CLIENT,
};

class Socket {
 public:
  Socket(){};
  Socket(int status) : socket_status(status){};
  Socket(int port_, std::string host_ip__) : port(port_), host_ip_(host_ip__){};
  virtual ~Socket(){};
  virtual int SetSocket(int) = 0;  // make socket return fd

 protected:
  struct sockaddr_in addr;
  int port;
  std::string host_ip_;

  int socket_status;  // enum
};

#endif
