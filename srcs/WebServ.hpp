#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Client.hpp"
#include "ISocket.hpp"
#include "Server.hpp"
#include "config.hpp"

class WebServ {
 public:
  typedef std::map<int, ISocket*>::iterator socket_iter;

  WebServ(const std::string& path);
  ~WebServ();

  void Activate(void);

  static const std::string default_path_;

 private:
  /* prohibit copy constructor and assignment operator */
  WebServ(const WebServ&);
  WebServ& operator=(const WebServ&);

  int HasUsableIO();

  int AcceptSession(socket_iter it);

  void ReadClient(socket_iter it);
  void ReadFile(socket_iter it);
  void ReadCGI(socket_iter it);
  void WriteCGI(socket_iter it);
  void WriteFile(socket_iter it);
  void WriteClient(socket_iter it);

  int ExecClientEvent(socket_iter it);

  void ParseConfig(const std::string& path);

  int max_fd_;
  fd_set rfd_set_, wfd_set_;
  struct timeval timeout_;

  std::map<int, ISocket*> sockets_;

  std::map<int, std::string> cgi_outputs_;
};

#endif
