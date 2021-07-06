#include "Server.hpp"

long Server::makeSocket(long _fd) {
  (void)_fd;
  long fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    throw std::runtime_error("socket error\n");

  memset((char *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(hostIp.c_str());
  addr.sin_port = htons(port);

  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");

  int on = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    throw std::runtime_error("bind error\n");

  if (listen(fd, 512) == -1) throw std::runtime_error("listen error\n");

  return fd;
}