#include "Server.hpp"
#include "WebServ.hpp"

int Server::SetSocket(void) {
  long fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    throw WebServ::StartingTimeException("SOCKET ERROR\n");

  memset((char *)&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(host_ip_.c_str());
  addr_.sin_port = htons(port_);

  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw WebServ::StartingTimeException("FCNTL ERROR\n");

  int on = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    throw WebServ::StartingTimeException("SETSOCKOPT ERROR\n");

  if (bind(fd, (struct sockaddr *)&addr_, sizeof(addr_)) == -1)
    throw WebServ::StartingTimeException("BIND ERROR\n");

  if (listen(fd, 512) == -1) throw WebServ::StartingTimeException("LISTEN ERROR\n");

  return fd;
}

const config::Config Server::GetConfig() {
  return config_;
}
