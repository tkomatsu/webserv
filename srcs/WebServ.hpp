#ifndef WEBSERV_HPP
#define WEBSERV_HPP

class WebServ;
#include "main.hpp"

class WebServ {
 public:
  WebServ(const std::string& path);
  ~WebServ();

  void start(void);

  static const std::string default_path;

 private:
  void parseConfig(const std::string& path);

  long max_fd;
  fd_set master_set;

  std::map<long, ISocket*> sockets;  // デストラクタでdeleteしときーや
  // std::vector<long> writable_client_fds;
};

#endif
