#include "Client.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include "CGI.hpp"
#include "utility.hpp"

const int Client::buf_max_ = 8192;

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

std::string Client::MakeAutoIndexContent(std::string dir_path) {
  DIR *dirp = opendir(dir_path.c_str());
  struct dirent *dp;
  fileinfo info;
  std::vector<fileinfo> index;

  while ((dp = readdir(dirp)) != NULL) {
    info.dirent_ = dp;
    stat(("./docs/" + std::string(dp->d_name)).c_str(), &info.stat_);
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
    tmp += response_.Now(info.stat_.st_mtimespec.tv_sec);
    tmp += std::string(19, ' ');
    if (S_ISREG(info.stat_.st_mode))
      tmp += ft::ltoa(info.stat_.st_size) + "\n";
    else
      tmp += "-\n";
  }
  tmp += "</pre><hr></body>\n</html>\n";

  return tmp;
}

// TODO: make good response_ content
void Client::Prepare(void) {
  // parse

  int ret;
  ret = READ_FILE;
  ret = WRITE_FILE;
  ret = WRITE_CGI;
  ret = WRITE_CLIENT;
  SetStatus((enum SocketStatus)ret);

  bool is_autoindex = true;

  switch (ret) {
    case READ_FILE:
      read_fd_ = open("./docs/html/index.html", O_RDONLY);
      fcntl(read_fd_, F_SETFL, O_NONBLOCK);

      response_.SetStatusCode(200);
      response_.AppendHeader("Content-Type", "text/html");
      response_.AppendHeader("Server", "Webserv");
      response_.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");
      break;

    case WRITE_FILE:
      write_fd_ = open("./docs/upload/post.html", O_RDWR | O_CREAT, 0644);
      fcntl(write_fd_, F_SETFL, O_NONBLOCK);

      response_.SetStatusCode(201);
      response_.AppendHeader("Content-Type", "text/html");
      response_.AppendHeader("Server", "Webserv");
      response_.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");
      response_.AppendHeader("Content-Location", "/post.html");
      break;

    case WRITE_CGI:
      GenProcessForCGI();

      response_.SetStatusCode(200);
      response_.AppendHeader("Content-Type", "text/html");
      response_.AppendHeader("Server", "Webserv");
      response_.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");
      break;

    case WRITE_CLIENT:
      if (is_autoindex) {
        response_.SetStatusCode(200);
        response_.AppendHeader("Content-Type", "text/html");
        response_.AppendHeader("Server", "Webserv");
        response_.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");

        std::string tmp = MakeAutoIndexContent("./docs");

        response_.SetBody(tmp);
      }
      break;
  }
}

int Client::recv(int client_fd) {
  int ret;
  char buf[Client::buf_max_] = {0};

  ret = ::recv(client_fd, buf, Client::buf_max_ - 1, 0);

  request_.AppendRawData(buf);

  if (ret >= 0) {
    size_t i = std::string(buf).find("\r\n\r\n");
    if (i != std::string::npos) {
      std::cout << "\nrecv from " + host_ip_ << ":" << port_ << std::endl;
    }
  }
  //} else {
  // TODO: if size of data exceed WebServ::buf_max_,
  // we need to stock them.
  //  }
  //}

  return ret;
}

int Client::send(int client_fd) {
  int ret;

  ret = ::send(client_fd, response_.Str().c_str(), response_.Str().size(), 0);
  if (ret > 0) {
    std::cout << "send to   " + host_ip_ << ":" << port_ << std::endl;
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

void Client::GenProcessForCGI(void) {
  CGI cgi_vals = CGI(request_);

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
