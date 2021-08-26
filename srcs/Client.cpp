#include "Client.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "CGI.hpp"
#include "utility.hpp"

const int Client::buf_max_ = 8192;

Client::Client(const config::Config &config) : config_(config) {
  socket_status_ = READ_CLIENT;
  sended_ = 0;
}

int Client::SetSocket(int _fd) {
  socklen_t len = sizeof(addr_);
  int fd = accept(_fd, (struct sockaddr *)&addr_, &len);

  port_ = ntohs(addr_.sin_port);
  host_ip_ = ft::inet_ntoa(addr_.sin_addr);

  if (fd == -1) throw std::runtime_error("accept error\n");
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");
  return fd;
}

void Client::SetEventStatus(enum SocketStatus status) {
  socket_status_ = status;
}

// ここの各準備処理を先々の関数に分けるのはやりづらい…
// 先々の関数は複数回ループが回るので、一回でいい操作(open)とかはここでやりたい
void Client::Prepare(void) {
  // temp
  int ret;
  // ret = READ_FILE;
  // ret = WRITE_FILE;
  // ret = WRITE_CGI;
  // ret = WRITE_CLIENT;
  ret = READ_WRITE_CGI;
  if (socket_status_ != WRITE_CLIENT) SetEventStatus((enum SocketStatus)ret);

  bool is_autoindex = true;

  if (ret == READ_FILE) {
    read_fd_ = open("./docs/html/index.html", O_RDONLY);
    if (read_fd_ == -1) throw std::runtime_error("open error\n");
    if (fcntl(read_fd_, F_SETFL, O_NONBLOCK) == -1)
      throw std::runtime_error("fcntl error\n");
  } else if (ret == WRITE_FILE) {
    write_fd_ = open("./docs/upload/post.html", O_RDWR | O_CREAT, 0644);
    fcntl(write_fd_, F_SETFL, O_NONBLOCK);
  } else if (ret == READ_WRITE_CGI) {
    GenProcessForCGI();
  } else if (ret == WRITE_CLIENT) {
    if (is_autoindex) response_.AutoIndexResponse("./docs/");
  }
}

int Client::RecvRequest(int client_fd) {
  int ret;
  char buf[buf_max_] = {0};

  ret = ::recv(client_fd, buf, buf_max_ - 1, 0);

  if (ret == -1) return ret;  // recv error
  if (ret == 0) return ret;   // closed by client

  request_.AppendRawData(buf);
  if (request_.GetStatus() == HttpMessage::DONE) {
    std::cout << "\nrecv from " << host_ip_ << ":" << port_ << std::endl;
    Prepare();
  }
  if (request_.GetStatus() == HttpMessage::DONE && !IsValidRequest()) {
    throw HttpResponseException("405");
  }
  return 1;
}

int Client::SendResponse(int client_fd) {
  int ret;

  size_t len = std::min((size_t)buf_max_, response_.Str().size() - sended_);
  ret = ::send(client_fd, &((response_.Str().c_str())[sended_]), len, 0);
  if (ret == -1) return ret;  // send error

  sended_ += ret;
  if (sended_ >= response_.Str().size()) {
    std::cout << "send to   " + host_ip_ << ":" << port_ << std::endl;
    response_.Clear();
    request_.Clear();
    sended_ = 0;
    SetEventStatus(READ_CLIENT);
  }
  return 1;  // continue send
}

int Client::ReadStaticFile() {
  int ret = 1;
  char buf[buf_max_] = {0};

  ret = read(read_fd_, buf, buf_max_ - 1);
  if (ret == -1) {
    close(read_fd_);
    return ret;
  } else if (ret == 0) {
    close(read_fd_);
    response_.AppendHeader("Content-Length",
                           ft::ltoa(response_.GetBody().size()));
    SetEventStatus(WRITE_CLIENT);
  } else {
    response_.AppendBody(buf);
  }
  return ret;
}

void Client::WriteStaticFile() {
  int ret = 1;

  size_t len = std::min((ssize_t)request_.GetBody().size(), (ssize_t)buf_max_);
  if (request_.GetMethod() == POST) {
    if ((ret = write(write_fd_, request_.GetBody().c_str(), len)) < 0)
      throw std::runtime_error("write error\n");
  }
  EraseRequestBody(ret);
  if (request_.GetBody().empty()) {
    close(write_fd_);
    response_.SetStatusCode(201);
    response_.AppendHeader("Content-Type", "text/html");
    response_.AppendHeader("Content-Location", "/post.html");
    SetEventStatus(WRITE_CLIENT);
  }
}

int Client::ReadCGIout() {
  int ret = 0;
  char buf[buf_max_] = {0};
  std::string headers;
  std::string body;

  ret = read(read_fd_, buf, buf_max_ - 1);
  if (ret < 0) {
    close(read_fd_);
    return ret;
  }
  response_.AppendRawData(buf);
  if (ret == 0) {
    response_.EndCGI();
    waitpid(-1, NULL, 0);
    close(read_fd_);
    response_.SetStatusCode(200);
    response_.AppendHeader("Content-Length",
                           ft::ltoa(response_.GetBody().length()));
    SetEventStatus(WRITE_CLIENT);
  }
  return ret;
}

void Client::WriteCGIin() {
  if (request_.GetMethod() == POST) {
    size_t len =
        std::min((ssize_t)request_.GetBody().size(), (ssize_t)buf_max_);
    int ret = 0;
    if ((ret = write(write_fd_, request_.GetBody().c_str(), len)) < 0)
      throw std::runtime_error("write error\n");
    EraseRequestBody(ret);
    if (request_.GetBody().empty()) {
      close(write_fd_);
      SetEventStatus(WRITE_CLIENT);
    }
  }
}

void Client::SetPipe(int *pipe_write, int *pipe_read) {
  if (pipe(pipe_write) == -1) throw std::runtime_error("pipe error\n");
  if (pipe(pipe_read) == -1) throw std::runtime_error("pipe error\n");
}

void Client::ExecCGI(int *pipe_write, int *pipe_read, char **args,
                     char **envs) {
  if (dup2(pipe_write[0], STDIN_FILENO) == -1)
    throw std::runtime_error("pipe error\n");
  if (dup2(pipe_read[1], STDOUT_FILENO) == -1)
    throw std::runtime_error("pipe error\n");

  close(pipe_write[0]);
  close(pipe_write[1]);
  close(pipe_read[0]);
  close(pipe_read[1]);

  execve("./docs/perl.cgi", args, envs);
  exit(EXIT_FAILURE);
}

void Client::GenProcessForCGI() {
  int pipe_write[2];
  int pipe_read[2];
  pid_t pid;

  SetPipe(pipe_write, pipe_read);

  if ((pid = fork()) == 0) {
    CGI cgi_vals = CGI(request_, port_, host_ip_, config_);
    ExecCGI(pipe_write, pipe_read, cgi_vals.GetArgs(), cgi_vals.GetEnvs());
  }

  write_fd_ = pipe_write[1];
  if (fcntl(write_fd_, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");
  close(pipe_write[0]);

  read_fd_ = pipe_read[0];
  if (fcntl(write_fd_, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");
  close(pipe_read[1]);
}

// TODO
bool Client::IsValidRequest(void) {
  if (request_.GetMethod() != GET) return false;
  return true;
}

void Client::HandleException(const char *err_msg) {
  response_.ErrorResponse(std::atoi(err_msg));
  SetEventStatus(WRITE_CLIENT);
}
