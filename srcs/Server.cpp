#include "Server.hpp"

int Server::SetSocket() {
  long fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    throw std::runtime_error("socket error\n");

  memset((char *)&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(host_ip_.c_str());
  addr_.sin_port = htons(port_);

  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");

  int on = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if (bind(fd, (struct sockaddr *)&addr_, sizeof(addr_)) == -1)
    throw std::runtime_error("bind error\n");

  if (listen(fd, 512) == -1) throw std::runtime_error("listen error\n");

  return fd;
}
