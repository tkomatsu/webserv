#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/stat.h>

#include <vector>

#include "Request.hpp"
#include "Response.hpp"
#include "Socket.hpp"

typedef struct fileinfo {
  struct dirent *dirent;
  struct stat stat;
} fileinfo;

class Client : public Socket {
 public:
  int SetSocket(int _fd);
  void Prepare(void);
  bool ParseRequest(void);
  void GenProcessForCGI(void);

  int recv(int client_fd);
  int send(int client_fd);

  static const int buf_max;

  void SetStatus(int status) { socket_status = status; }

  Response &GetResponse() { return response; };
  int GetStatus() { return socket_status; };
  int GetWriteFd() { return write_fd_; };
  int GetReadFd() { return read_fd_; };

 private:
  Request request;
  Response response;

  int write_fd_;
  int read_fd_;

  std::vector<fileinfo> index;
};

#endif
