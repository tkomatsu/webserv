#ifndef ISOCKET_HPP
#define ISOCKET_HPP

#include <netinet/in.h>

#include <string>

#include "utility.hpp"

class ISocket {
 public:
  ISocket(){};
  ISocket(int port, std::string host_ip) : port_(port), host_ip_(host_ip){};
  virtual ~ISocket(){};

 protected:
  struct sockaddr_in addr_;
  int port_;
  std::string host_ip_;

  enum SocketStatus socket_status_;

 private: /* prohibit */
  ISocket(const ISocket&);
  ISocket& operator=(const ISocket&);
};

#endif
