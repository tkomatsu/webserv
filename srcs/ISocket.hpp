#ifndef ISOCKET_HPP
#define ISOCKET_HPP

class ISocket {
 public:
  virtual ~ISocket(){};
  virtual int SetSocket(int) = 0;  // make socket return fd
};

#endif
