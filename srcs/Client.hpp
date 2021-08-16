#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/stat.h>

#include <vector>

#include "ISocket.hpp"
#include "Request.hpp"
#include "Response.hpp"

typedef struct fileinfo {
  struct dirent *dirent_;
  struct stat stat_;
} fileinfo;

class Client : public Socket {
 public:
  Client() { socket_status_ = READ_CLIENT; };

  int SetSocket(int _fd);
  void Prepare(void);
  bool ParseRequest(void);
  void GenProcessForCGI(int client_port, std::string client_host, int server_port, std::string server_host);

  int recv(int client_fd);
  int send(int client_fd);

  const Response &GetResponse() { return response_; };
  const Request &GetRequest() { return request_; };
  int GetStatus() { return socket_status_; };
  int GetWriteFd() { return write_fd_; };
  int GetReadFd() { return read_fd_; };
  int GetPort() { return port_; };
  std::string GetHostIp() { return host_ip_; };
  const std::string& GetResponseBody() const { return response_.GetBody(); };

  void AppendResponseBody(std::string buf) { response_.AppendBody(buf); };
  void AppendResponseHeader(std::string key, std::string val) {
    response_.AppendHeader(key, val);
  };
  void AppendResponseHeader(std::pair<std::string, std::string> header) {
    response_.AppendHeader(header);
  };
  void AppendResponseRawData(std::string data, bool is_continue) {
    response_.AppendRawData(data);
    if (!is_continue) response_.EndCGI();
  };

  void SetResponseBody(std::string buf) { response_.SetBody(buf); };
  void SetStatus(enum SocketStatus status);
  void SetServerPort(int port) {server_port_ = port;};
  void SetServerHost(std::string server_host_ip) {server_host_ip_ = server_host_ip;};

  static const int buf_max_;
 private:
  void SetPipe(int *pipe_write, int *pipe_read);
  void ExecCGI(int *pipe_write, int *pipe_read, char **args, char **envs);

  enum SocketStatus GetNextOfReadClient();

  std::string MakeAutoIndexContent(std::string dir_path);

  Request request_;
  Response response_;

  int write_fd_;
  int read_fd_;

  int server_port_;
  std::string server_host_ip_;
};

#endif /* CLIENT_HPP */
