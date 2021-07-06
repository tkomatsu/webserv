#ifndef ISOCKET_HPP
#define ISOCKET_HPP

class ISocket {
 public:
  virtual ~ISocket(){};
  virtual long makeSocket(long) = 0;  // make socket return fd
};

#endif
