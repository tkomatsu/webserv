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
  static const int buf_max;

 private:
  void parseConfig(const std::string& path);

  int max_fd;

  std::map<long, ISocket*> sockets;  // デストラクタでdeleteしときーや
};

#endif
