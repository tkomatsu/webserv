#ifndef WEBSERV_HPP
#define WEBSERV_HPP

class WebServ;
#include "main.hpp"

class WebServ {
 public:
  static const std::string default_path;
  static const int buf_max;

  void parse(const std::string& path);
  void start(void);

 private:
  std::vector<Server> servers;
  int max_fd;
  fd_set master_set;
};

#endif
