#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>

#include "CGI.hpp"
#include "ISocket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "config.hpp"

class Client : public Socket {
 public:
  static const int buf_max_;

  Client(const config::Config &config);

  int SetSocket(int _fd);

  // I/O actions
  int RecvRequest(int client_fd);
  int ReadStaticFile();
  int ReadCGIout();
  int SendResponse(int client_fd);
  void WriteStaticFile();
  void WriteCGIin();

  // Getter
  int GetStatus() const { return socket_status_; };
  int GetWriteFd() const { return write_fd_; };
  int GetReadFd() const { return read_fd_; };

  // Exception handling
  int HandleException(const char *err_msg);

 private:
  void EraseRequestBody(ssize_t length) { request_.EraseBody(length); };
  void SetEventStatus(enum SocketStatus status);
  void Prepare(void);
  void GenProcessForCGI(void);
  void SetPipe(int *pipe_write, int *pipe_read);
  void ExecCGI(int *pipe_write, int *pipe_read, const CGI &cgi);
  bool IsValidRequest();

  // member variables
  Request request_;
  Response response_;
  size_t sended_;  // num of chars sended to client
  int write_fd_;
  int read_fd_;
  const config::Config config_;
};

#endif /* CLIENT_HPP */
