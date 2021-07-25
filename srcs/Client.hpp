#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/stat.h>

#include <vector>

#include "Request.hpp"
#include "Response.hpp"
#include "Socket.hpp"

typedef struct fileinfo {
  struct dirent *dirent_;
  struct stat stat_;
} fileinfo;

class Client : public Socket {
 public:
  Client() : Socket(READ_CLIENT){};

  int SetSocket(int _fd);
  void Prepare(void);
  bool ParseRequest(void);
  void GenProcessForCGI(void);

  int recv(int client_fd);
  int send(int client_fd);

  static const int buf_max_;

  const Response &GetResponse() { return response_; };
  int GetStatus() { return socket_status_; };
  int GetWriteFd() { return write_fd_; };
  int GetReadFd() { return read_fd_; };

  void SetResponseBody(std::string buf) { response_.SetBody(buf); };
  void AppendResponseHeader(std::string key, std::string val) {response_.AppendHeader(key, val); };

  std::string MakeAutoIndexContent(std::string dir_path);

 private:
  void SetPipe(int *pipe_write, int *pipe_read);
  void ExecCGI(int *pipe_write, int *pipe_read, char **args, char **envs);

  Request request_;
  Response response_;

  int write_fd_;
  int read_fd_;
};

#endif
