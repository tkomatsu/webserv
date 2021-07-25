#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

#include <string>

#include "utility.hpp"

class Socket {
 public:
  Socket(){};
  Socket(enum SocketStatus status) { socket_status_ = status; };
  Socket(int port, std::string host_ip) : port_(port), host_ip_(host_ip){};
  virtual ~Socket(){};

  virtual int SetSocket(int _fd);
  void SetStatus(enum SocketStatus status);

 protected:
  struct sockaddr_in addr_;
  int port_;
  std::string host_ip_;

  enum SocketStatus socket_status_;
};

#endif
