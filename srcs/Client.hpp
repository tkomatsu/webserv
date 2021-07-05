#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "main.hpp"

class Client : public ISocket {
 public:
  long makeSocket(long _fd);

 private:
};

#endif
