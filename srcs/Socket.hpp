#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

#include <string>

#include "utility.hpp"

class Socket {
 public:
  Socket(){};
  Socket(int port, std::string host_ip) : port_(port), host_ip_(host_ip){};
  virtual ~Socket(){};

  void SetStatus(enum SocketStatus status);

 protected:
  struct sockaddr_in addr_;
  int port_;
  std::string host_ip_;

  enum SocketStatus socket_status_;

 private: /* prohibit */
  Socket(const Socket&);
  Socket& operator=(const Socket&);
};

#endif
