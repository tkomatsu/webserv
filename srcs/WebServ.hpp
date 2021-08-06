#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>
#include <string>

#include "ISocket.hpp"

class Client;

class WebServ {
 public:
  typedef std::map<int, Socket*>::iterator socket_iter;

 public:
  WebServ(const std::string& path);
  ~WebServ();

  void Activate(void);

  static const std::string default_path_;
  static const int buf_max_;

 private:
  int HasUsableIO();

  int AcceptSession(socket_iter it);

  int ReadClient(socket_iter it);
  int ReadFile(socket_iter it);
  int ReadCGI(socket_iter it);
  int WriteCGI(socket_iter it);
  int WriteFile(socket_iter it);
  int WriteClient(socket_iter it);

  int ExecClientEvent(socket_iter it);

  void ParseConig(const std::string& path);

  int max_fd_;
  fd_set rfd_set_, wfd_set_;
  struct timeval timeout_;

  std::map<int, Socket*> sockets_;

  std::map<int, std::string> cgi_outputs_;
};

#endif
