#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>
#include <string>

#include "Socket.hpp"

class WebServ {
 public:
  typedef std::map<int, Socket*>::iterator socket_iter;

 public:
  WebServ(const std::string& path);
  ~WebServ();

  void Activate(void);

  static const std::string default_path;
  static const int buf_max;

 private:
  int HasUsableIO();

  int AcceptSession(socket_iter it);

  int ReadClient(socket_iter it);
  int ReadFile(socket_iter it);
  int ReadCGI(socket_iter it);
  int WriteCGI(socket_iter it);
  int WriteFile(socket_iter it);
  int WriteClient(socket_iter it);

  void ExecClientEvent(socket_iter it);

  void ParseConig(const std::string& path);

  int max_fd;
  fd_set rfd_set, wfd_set;
  struct timeval timeout;

  std::map<int, Socket*> sockets;
};

#endif
