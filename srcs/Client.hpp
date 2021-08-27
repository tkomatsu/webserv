#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdexcept>
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
  void HandleException(const char *err_msg);

 private:
  void EraseRequestBody(ssize_t length) { request_.EraseBody(length); };
  void SetEventStatus(enum SocketStatus status);
  void Prepare(void);
  void GenProcessForCGI(void);
  void SetPipe(int *pipe_write, int *pipe_read);
  void ExecCGI(int *pipe_write, int *pipe_read, const CGI &cgi);
  bool IsValidRequest();
  bool IsValidExtension(std::string path_uri, std::string request_path);
  enum SocketStatus GetNextOfReadClient();
  std::string MakePathUri(std::string alias_path, std::string request_uri,
                          std::string location_path);

  // member variables
  Request request_;
  Response response_;
  size_t sended_;  // num of chars sended to client
  int write_fd_;
  int read_fd_;
  const config::Config config_;

 public:
  class HttpResponseException : public std::runtime_error {
   public:
    HttpResponseException(const std::string &what) : std::runtime_error(what) {}
  };
};

#endif /* CLIENT_HPP */
