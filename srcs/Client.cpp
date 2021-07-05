#include "Client.hpp"

long Client::makeSocket(long _fd) {
  long fd = accept(_fd, NULL, NULL);

  if (fd == -1) throw std::runtime_error("accept error\n");
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");
  return fd;
}