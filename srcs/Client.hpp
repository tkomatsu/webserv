#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "main.hpp"

enum SocketStatus {
  READ_CLIENT,
  READ_FILE,
  READ_CGI,
  WRITE_CGI,
  WRITE_FILE,
  WRITE_CLIENT,
};

class Client : public ISocket {
 public:
  long makeSocket(long _fd);
  void makeResponse(void);
  bool ParseRequest(void);
  void GenProcessForCGI(void);

  long recv(long client_fd);
  long send(long client_fd);

  static const int buf_max;

  Response &GetResponse() { return response; };
  
  int socket_status;

  int write_cgi_fd;
  int read_cgi_fd;

 private:
  Request request;
  Response response;

  struct sockaddr_in addr;
  int port;
  std::string hostIp;

};

#endif
