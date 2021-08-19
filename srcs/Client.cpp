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

enum SocketStatus Client::GetNextOfReadClient() {
  enum SocketStatus ret;

  ret = WRITE_CGI;

  switch (request_.GetMethod()) {
    case GET:
      // if (config.GetAllowedMethods(request_.GetURI()).find(GET) == false)
      //   throw 405

      // (CGI)
      // std::vector<std::string> request_uri = ft::vsplit(
      //     request_.GetURI(), '?');  // /abc?mcgee=mine => ["/abc",
      //     "mcgee=mine"]
      // path = alias + (request_uri[0] - location)
      // if path's extension in config.GetExtensions():
      //   close(write_fd);
      //   ret = READ_CGI;
      //   break;

      // (SIMPLE GET)
      // struct stat buffer;
      // if (stat (name.c_str(), &buffer) == -1)
      //   throw 404;
      // if (S_ISREG(buffer.st_mode))
      //   ret = READ_FILE;
      //   break;
      // else if (S_ISDIR(buffer.st_mode)) {
      //    (GET INDEX)
      //    if (files in dir) in config.GetIndexes()
      //       modify path to the index file
      //       ret = READ_FILE; (read index file)
      //       break;
      //    (AUTOINDEX)
      //    else if (config.GetAutoIndex() == true)
      //       ret = WRITE_CLIENT;
      //       break;
      // }
      // throw 404

    case POST:
      // if (config.GetAllowedMethods(request_.GetURI()).find(POST) == false)
      //   throw 405

      // (CGI)
      // std::vector<std::string> request_uri = ft::vsplit(
      //     request_.GetURI(), '?');  // /abc?mcgee=mine => ["/abc", "mcgee=mine"]
      // path = alias + (request_uri[0] - location)
      // if path's extension in config.GetExtensions():
      //   ret = READ_WRITE_CGI;
      //   break;

      // (upload)
      // if upload_store & upload_pass:
      //   if request_.GetURI() == upload_pass
      //      ret = WRITE_FILE;
      //      break;

      //  throw 405

    case DELETE:
      // if (config.GetAllowedMethods(request_.GetURI()).find(DELETE) == false)
      //   throw 405

      // remove(filepath);
      // ret = WRITE_CLIENT;

    case INVALID:
      // throw 405
    default:
      break;
  }

  return ret;
}

// TODO: make good response_ content
void Client::Prepare(void) {
  enum SocketStatus ret;

  ret = GetNextOfReadClient();
  SetStatus(ret);

  // std::vector<std::string> request_uri = ft::vsplit(
  //   request_.GetURI(), '?');  // /abc?mcgee=mine => ["/abc", "mcgee=mine"]

  // std::cout << request_uri[0] << std::endl;
  // if (request_uri.size() >= 1)
  // std::cout << request_uri[1] << std::endl;

  if (ret == READ_FILE) {
      // alias: ./docs/html/  config.GetAlias(request_uri[0])
      // location: /  config.GetLocation(request_uri[0])
      // request: /abc  request_uri[0]

      // open(alias + (request-path - location))
      read_fd_ = open("./docs/html/index.html", O_RDONLY);
      fcntl(read_fd_, F_SETFL, O_NONBLOCK);
      response_.SetStatusCode(200);
      response_.AppendHeader("Content-Type", "text/html");
  } else if (ret == WRITE_FILE) {
    // open(upload_store)
    write_fd_ = open("./docs/upload/post.html", O_RDWR | O_CREAT, 0644);
    fcntl(write_fd_, F_SETFL, O_NONBLOCK);

    response_.SetStatusCode(201);
    response_.AppendHeader("Content-Type", "text/html");
    response_.AppendHeader("Content-Location", "/post.html");
  } else if (ret == WRITE_CGI) {
    GenProcessForCGI(this->port_, this->host_ip_, config_.port, config_.host);

    response_.SetStatusCode(200);
    response_.AppendHeader("Content-Type", "text/html");
  } else if (ret == WRITE_CLIENT) {
    if (1 /* is autoindex */) {
      response_.SetStatusCode(200);
      response_.AppendHeader("Content-Type", "text/html");

      // MakeAutoIndexContent(request-path)
      std::string tmp = MakeAutoIndexContent("./docs/");

      response_.SetBody(tmp);
    }
  }
}

int Client::recv(int client_fd) {
  int ret;
  char buf[Client::buf_max_] = {0};

  ret = ::recv(client_fd, buf, Client::buf_max_ - 1, 0);

  if (ret == -1) return ret;  // recv error
  if (ret == 0) return ret;   // closed by client

  request_.AppendRawData(buf);

  return 1;
}

int Client::send(int client_fd) {
  int ret;

  ret = ::send(client_fd, response_.Str().c_str(), response_.Str().size(), 0);
  if (ret > 0) {
    std::cout << "send to   " + host_ip_ << ":" << port_ << std::endl;
  }
  response_.Clear();
  request_.Clear();
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

  // execve(alias + (request-path - location), args, envs)
  execve("./docs/perl.cgi", args, envs);
  exit(EXIT_FAILURE);
}

void Client::GenProcessForCGI(int client_port, std::string client_host, int server_port, std::string server_host) {
  CGI cgi_vals = CGI(request_, client_port, client_host, server_port, server_host);

  int pipe_write[2];
  int pipe_read[2];
  pid_t pid;

  SetPipe(pipe_write, pipe_read);

  if ((pid = fork()) == 0) {
    ExecCGI(pipe_write, pipe_read, cgi_vals.GetArgs(), cgi_vals.GetEnvs());
  }

  write_fd_ = pipe_write[1];
  fcntl(write_fd_, F_SETFL, O_NONBLOCK);
  close(pipe_write[0]);

  read_fd_ = pipe_read[0];
  fcntl(read_fd_, F_SETFL, O_NONBLOCK);
  close(pipe_read[1]);
}

// connection: keep-aliveはつけっぱでいい