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
  int SetSocket(int _fd);
  void makeResponse(void);
  bool ParseRequest(void);
  void GenProcessForCGI(void);

  int recv(int client_fd);
  int send(int client_fd);

  static const int buf_max;

  Response &GetResponse() { return response; };
  int GetStatus() { return socket_status; };
  void SetStatus(int status) { socket_status = status; }
  int GetWriteFileFd() { return write_file_fd; };
  int GetReadFileFd() { return read_file_fd; };
  int GetWriteCgiFd() { return write_cgi_fd; };
  int GetReadCgiFd() { return read_cgi_fd; };

 private:
  Request request;
  Response response;

  struct sockaddr_in addr;
  int port;
  std::string hostIp;

  int socket_status;  // enum

  int write_file_fd;
  int read_file_fd;

  int write_cgi_fd;
  int read_cgi_fd;
};

#endif
