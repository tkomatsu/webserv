#include "Server.hpp"

#include <netdb.h>

#include <stdexcept>

int Server::OpenListenSocket() {
  struct addrinfo hints, *listp, *p;
  int listenfd, optval = 1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
  hints.ai_flags |= AI_NUMERICSERV;

  char port[20];
  sprintf(port, "%d", port_);
  int rn = getaddrinfo(NULL, port, &hints, &listp);
  if (rn != 0)
    throw std::runtime_error("getaddrinfo: " + std::string(gai_strerror(rn)));

  for (p = listp; p; p = p->ai_next) {
    if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
      continue;

    if (fcntl(listenfd, F_SETFL, O_NONBLOCK) != 0)
      throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval)) < 0)
      throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));

    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    if (close(listenfd) < 0)
      throw std::runtime_error("close: " + std::string(strerror(errno)));
  }

  freeaddrinfo(listp);
  if (!p) return -1;

  if (listen(listenfd, 1024) < 0) {
    if (close(listenfd) < 0)
      throw std::runtime_error("close: " + std::string(strerror(errno)));
    return -1;
  }
  return listenfd;
}

const config::Config &Server::GetConfig() const { return config_; }
