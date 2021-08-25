#include "Client.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include "CGI.hpp"
#include "utility.hpp"

const int Client::buf_max_ = 8192;

Client::Client(const struct config::Config config) : config_(config) {
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

void Client::AppendResponseBody(std::string buf) { response_.AppendBody(buf); };

void Client::AppendResponseHeader(std::string key, std::string val) {
  response_.AppendHeader(key, val);
};

void Client::AppendResponseHeader(std::pair<std::string, std::string> header) {
  response_.AppendHeader(header);
};

void Client::AppendResponseRawData(std::string data, bool is_continue) {
  response_.AppendRawData(data);
  if (!is_continue) response_.EndCGI();
};

void Client::SetStatus(enum SocketStatus status) { socket_status_ = status; }

std::string Client::MakeAutoIndexContent(std::string dir_path) {
  DIR *dirp = opendir(dir_path.c_str());
  if (dirp == NULL) return "";
  struct dirent *dp;
  std::vector<fileinfo> index;

  while ((dp = readdir(dirp)) != NULL) {
    fileinfo info;

    info.dirent_ = dp;
    stat((dir_path + std::string(dp->d_name)).c_str(), &info.stat_);
    index.push_back(info);
  }
  closedir(dirp);
  std::string tmp;
  tmp += "<html>\n<head><title>Index of ";
  tmp += "/";
  tmp += "</title></head>\n<body bgcolor=\"white\">\n<h1>Index of ";
  tmp += "/</h1><hr><pre><a href=\"../\">../</a>\n";

  for (std::vector<fileinfo>::iterator it = index.begin(); it != index.end();
       ++it) {
    fileinfo info = *it;

    if (info.dirent_->d_name[0] == '.') continue;

    tmp += "<a href=\"" + std::string(info.dirent_->d_name) + "/\">";
    if (std::string(info.dirent_->d_name).length() >= 50)
      tmp += std::string(info.dirent_->d_name).substr(0, 47) + "..&gt;";
    else {
      tmp += std::string(info.dirent_->d_name);
      if (S_ISDIR(info.stat_.st_mode)) tmp += "/";
    }
    tmp +=
        "</a>" +
        std::string(50 - std::min((size_t)50,
                                  std::string(info.dirent_->d_name).length()),
                    ' ');
    if (!S_ISDIR(info.stat_.st_mode)) tmp += " ";
    tmp += ft::AutoIndexNow(info.stat_.st_mtimespec.tv_sec);
    tmp += std::string(19, ' ');
    if (S_ISREG(info.stat_.st_mode))
      tmp += ft::ltoa(info.stat_.st_size) + "\n";
    else
      tmp += "-\n";
  }
  tmp += "</pre><hr></body>\n</html>\n";

  return tmp;
}

// ここの各準備処理を先々の関数に分けるのはやりづらい…
// 先々の関数は複数回ループが回るので、一回でいい操作(open)とかはここでやりたい
void Client::Prepare(void) {
  int ret;
  ret = READ_FILE;
  // ret = WRITE_FILE;
  // ret = WRITE_CGI;
  // ret = WRITE_CLIENT;
  // ret = READ_WRITE_CGI;
  SetStatus((enum SocketStatus)ret);

  bool is_autoindex = true;

  if (ret == READ_FILE) {
    read_fd_ = open("./docs/html/index.html", O_RDONLY);
    if (read_fd_ == -1) throw std::runtime_error("open error\n");
    if (fcntl(read_fd_, F_SETFL, O_NONBLOCK) == -1)
      throw std::runtime_error("fcntl error\n");
    response_.AppendHeader("Content-Type", "text/html");

  } else if (ret == WRITE_FILE) {
    write_fd_ = open("./docs/upload/post.html", O_RDWR | O_CREAT, 0644);
    fcntl(write_fd_, F_SETFL, O_NONBLOCK);

    response_.SetStatusCode(201);
    response_.AppendHeader("Content-Type", "text/html");
    response_.AppendHeader("Content-Location", "/post.html");
  } else if (ret == READ_WRITE_CGI) {
    GenProcessForCGI();

    response_.SetStatusCode(200);
    response_.AppendHeader("Content-Type", "text/html");
  } else if (ret == WRITE_CLIENT) {
    if (is_autoindex) {
      response_.SetStatusCode(200);
      response_.AppendHeader("Content-Type", "text/html");

      std::string tmp = MakeAutoIndexContent("./docs/");
      if (tmp.empty()) {
        // 404
      }

      response_.SetBody(tmp);
    }
  }
}

int Client::RecvRequest(int client_fd) {
  int ret;
  char buf[Client::buf_max_] = {0};

  ret = ::recv(client_fd, buf, Client::buf_max_ - 1, 0);

  if (ret == -1) return ret;  // recv error
  if (ret == 0) return ret;   // closed by client

  request_.AppendRawData(buf);
  if (request_.GetStatus() == HttpMessage::DONE && !IsValidRequest()) {
    response_.ErrorResponse(405);
    SetStatus(WRITE_CLIENT);
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
    SetStatus(READ_CLIENT);
    return 1;  // all sended
  }
  return 2;  // continue send
}

int Client::ReadStaticFile() {
  int ret = 1;
  char buf[buf_max_] = {0};

  ret = read(GetReadFd(), buf, buf_max_ - 1);
  if (ret == -1) {
    close(GetReadFd());
    return ret;
  } else if (ret == 0) {
    close(GetReadFd());
    // TODO: エラーレスポンスのヘッダーとかぶるから消したい。
    AppendResponseHeader("Content-Length",
                         ft::ltoa(response_.GetBody().size()));
    SetStatus(WRITE_CLIENT);
  } else {
    AppendResponseBody(buf);
  }
  return ret;
}

// TODO
int Client::WriteStaticFile() {
  int ret = 1;

  size_t len = std::min((ssize_t)request_.GetBody().size(), (ssize_t)buf_max_);
  if (request_.GetMethod() == POST) {
    if ((ret = write(GetWriteFd(), request_.GetBody().c_str(), len)) < 0)
      throw std::runtime_error("write error\n");
  }

  EraseRequestBody(ret);
  if (GetRequestBody().empty()) {
    close(GetWriteFd());
    SetStatus(WRITE_CLIENT);
  }
  return ret;
}

int Client::ReadCGIout() {
  int ret = 1;
  char buf[buf_max_] = {0};
  std::string headers;
  std::string body;

  ret = read(GetReadFd(), buf, buf_max_ - 1);
  if (ret < 0) {
    close(GetReadFd());
    return ret;
  }
  AppendResponseRawData(buf, ret);
  if (ret == 0) {
    close(GetReadFd());
    // TODO: エラーレスポンスのヘッダーとかぶるから消したい。
    AppendResponseHeader("Content-Length",
                         ft::ltoa(response_.GetBody().length()));
    SetStatus(WRITE_CLIENT);
  }
  return ret;
}

// TODO
int Client::WriteCGIin() {
  int ret = 1;

  size_t len = std::min((ssize_t)request_.GetBody().size(), (ssize_t)buf_max_);
  if (GetRequest().GetMethod() == POST) {
    if ((ret = write(GetWriteFd(), request_.GetBody().c_str(), len)) < 0)
      throw std::runtime_error("write error\n");
    EraseRequestBody(ret);
  }

  if (GetRequestBody().empty()) {
    close(GetWriteFd());
    SetStatus(WRITE_CLIENT);
  }
  return ret;
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
  CGI cgi_vals = CGI(request_, this->port_, this->host_ip_, config_);

  int pipe_write[2];
  int pipe_read[2];
  pid_t pid;

  SetPipe(pipe_write, pipe_read);

  if ((pid = fork()) == 0) {
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
