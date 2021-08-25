#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/stat.h>

#include <stdexcept>
#include <vector>

#include "ISocket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "config.hpp"

typedef struct fileinfo {
  struct dirent *dirent_;
  struct stat stat_;
} fileinfo;

class Client : public Socket {
 public:
  static const int buf_max_;

  Client(const struct config::Config config);

  int SetSocket(int _fd);
  void Prepare(void);
  bool ParseRequest(void);
  void GenProcessForCGI(void);

  // I/O actions
  int RecvRequest(int client_fd);
  int ReadStaticFile();
  int ReadCGIout();
  int SendResponse(int client_fd);
  void WriteStaticFile();
  void WriteCGIin();

  // Getter
  const Response &GetResponse() { return response_; };
  const Request &GetRequest() { return request_; };
  int GetStatus() { return socket_status_; };
  int GetWriteFd() { return write_fd_; };
  int GetReadFd() { return read_fd_; };
  int GetPort() { return port_; };
  std::string GetHostIp() { return host_ip_; };

  const std::string &GetResponseBody() const { return response_.GetBody(); };
  const std::string &GetRequestBody() { return request_.GetBody(); };

  // Setter
  void AppendResponseBody(std::string buf);
  void AppendResponseHeader(std::string key, std::string val);
  void AppendResponseHeader(std::pair<std::string, std::string> header);
  void AppendResponseRawData(std::string data, bool is_continue);

  void SetResponseBody(std::string buf) { response_.SetBody(buf); };
  void SetStatus(enum SocketStatus status);

  std::string MakeAutoIndexContent(std::string dir_path);

  // Clear
  void EraseRequestBody(ssize_t length) { request_.EraseBody(length); };
  void ClearResponse(void) { response_.Clear(); }

 private:
  void SetPipe(int *pipe_write, int *pipe_read);
  void ExecCGI(int *pipe_write, int *pipe_read, char **args, char **envs);
  bool IsValidRequest();

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
