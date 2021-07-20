#ifndef WEBSERV_HPP
#define WEBSERV_HPP

class WebServ;
#include "main.hpp"

class WebServ {
 public:
  typedef std::map<int, Socket*>::iterator map_iter;

 public:
  WebServ(const std::string& path);
  ~WebServ();

  void Activate(void);
  int HasUsableIO();

  int AcceptSession(map_iter it);

  int ReadClient(map_iter it);
  int ReadFile(map_iter it);
  int ReadCGI(map_iter it);
  int WriteCGI(map_iter it);
  int WriteFile(map_iter it);
  int WriteClient(map_iter it);

  static const std::string default_path;
  static const int buf_max;

 private:
  void ParseConig(const std::string& path);

  int max_fd;
  fd_set rfd_set, wfd_set;
  struct timeval timeout;

  std::map<int, Socket*> sockets;  // デストラクタでdeleteしときーや
};

#endif
