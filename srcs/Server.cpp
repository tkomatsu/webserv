#include "Server.hpp"

Server::Server(const config::Config& config)
    : ISocket(config.GetPort(), config.GetHost()), config_(config){};

Server::~Server(){};

int Server::OpenListenSocket() {
  struct addrinfo hints, *listp, *p;
  int listenfd, optval = 1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
  hints.ai_flags |= AI_NUMERICSERV;

  char port[20];
  sprintf(port, "%d", port_);
  int rn = getaddrinfo(host_ip_.c_str(), port, &hints, &listp);
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
  if (!p) throw std::runtime_error("could not bind");

  if (listen(listenfd, 1024) < 0) {
    if (close(listenfd) < 0)
      throw std::runtime_error("close: " + std::string(strerror(errno)));
    throw std::runtime_error("listen: " + std::string(strerror(errno)));
  }
  return listenfd;
}

const config::Config& Server::GetConfig() const { return config_; }
