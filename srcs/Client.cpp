#include "Client.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <string>

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

bool Client::IsValidExtension(std::string path_uri, std::string request_path) {
  size_t i = path_uri.length() - 1;

  while (i > 0 && path_uri[i] != '.') --i;
  std::string extension = path_uri.substr(i);

  std::set<std::string> allowd_extensions;
  allowd_extensions = config_.GetExtensions(request_path);

  if (allowd_extensions.count(extension) == 1)
    return true;
  else
    return false;
}

std::string Client::GetIndexFileIfExist(std::string path_uri,
                                        std::string request_path) {
  std::vector<std::string> conf_indexes = config_.GetIndexes(request_path);

  for (size_t i = 0; i < conf_indexes.size(); ++i) {
    struct stat buffer;
    std::string filename = conf_indexes[i];

    if (stat((path_uri + filename).c_str(), &buffer) != -1) {
      return filename;
    }
  }
  return "";
}

enum SocketStatus Client::GetNextOfReadClient(std::string *path_uri) {
  enum SocketStatus ret = WRITE_CLIENT;

  std::vector<std::string> request_uri = ft::vsplit(request_.GetURI(), '?');

  if (request_uri.size() > 1) std::cout << request_uri[1] << std::endl;
  std::string request_path = request_uri[0];
  std::string alias = config_.GetAlias(request_path);
  std::string location_path = config_.GetPath(request_path);

  *path_uri = MakePathUri(alias, request_path, location_path);

  switch (request_.GetMethod()) {
    case GET:
      if (config_.GetAllowedMethods(request_path).count(GET) == 0) {
        //  throw 405
        break;
      }

      // (CGI)
      if (IsValidExtension(*path_uri, request_path)) {
        ret = READ_WRITE_CGI;
        break;
      }

      // (SIMPLE GET)
      struct stat buffer;
      if (stat((*path_uri).c_str(), &buffer) == -1) {
        std::cout << "わんだらa" << std::endl;
        //   throw 404;
        break;
      }

      if (S_ISREG(buffer.st_mode)) {
        std::cout << "わんだらb" << std::endl;
        ret = READ_FILE;
        break;
      } else if (S_ISDIR(buffer.st_mode)) {
        std::cout << "わんだら" << std::endl;

        *path_uri += "/";

        std::string index_filename =
            GetIndexFileIfExist(*path_uri, request_path);
        if (index_filename.empty() == false) {
          // (GET INDEX)
          *path_uri += index_filename;
          ret = READ_FILE;
          break;
        } else if (config_.GetAutoindex(request_path) == true) {
          // (AUTOINDEX)
          ret = WRITE_CLIENT;
          break;
        }
      }
      std::cout << "わんだらz" << std::endl;

      // throw 404
      break;

    case POST:
      // if (config.GetAllowedMethods(request_.GetURI()).find(POST) ==
      // false)
      //   throw 405

      // (CGI)
      // std::vector<std::string> request_uri = ft::vsplit(
      //     request_.GetURI(), '?');  // /abc?mcgee=mine => ["/abc",
      //     "mcgee=mine"]
      // path = alias + (request_uri[0] - location)
      // if path's extension in config.GetExtensions():
      ret = READ_WRITE_CGI;
      //   break;

      // (upload)
      // if upload_store & upload_pass:
      //   if request_.GetURI() == upload_pass
      //      ret = WRITE_FILE;
      //      break;

      //  throw 405
      break;
    case DELETE:
      // if (config.GetAllowedMethods(request_.GetURI()).find(DELETE) ==
      // false)
      //   throw 405

      // remove(filepath);
      // ret = WRITE_CLIENT;
      break;
    case INVALID:
      // throw 405
      break;
  }

  return ret;
}

void Client::Prepare(void) {
  if (socket_status_ != READ_CLIENT) return;

  enum SocketStatus ret;
  std::string path_uri;

  ret = GetNextOfReadClient(&path_uri);
  SetEventStatus(ret);

  bool is_autoindex = true;
  std::cout << "path_uri is " << path_uri << std::endl;

  if (ret == READ_FILE) {
    if ((read_fd_ = open(path_uri.c_str(), O_RDONLY)) < 0)
      throw ft::HttpResponseException("500");
    if (fcntl(read_fd_, F_SETFL, O_NONBLOCK) == -1)
      throw ft::HttpResponseException("500");
  } else if (ret == WRITE_FILE) {
    if ((write_fd_ = open(path_uri.c_str(), O_RDWR | O_CREAT, 0644)) < 0)
      throw ft::HttpResponseException("500");
    if (fcntl(write_fd_, F_SETFL, O_NONBLOCK) < 0)
      throw ft::HttpResponseException("500");

  } else if (ret == READ_WRITE_CGI) {
    GenProcessForCGI();
  } else if (ret == WRITE_CLIENT) {
    if (is_autoindex) response_.AutoIndexResponse(path_uri.c_str());
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
    throw ft::HttpResponseException("405");
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
      throw ft::HttpResponseException("500");
  }

  EraseRequestBody(ret);
  if (request_.GetBody().empty()) {
    close(write_fd_);
    response_.SetStatusCode(201);
    response_.AppendHeader("Content-Type", "text/html");
    response_.AppendHeader("Content-Location", "/post.html");

    response_.AppendHeader("Content-Length", "0");

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
      throw ft::HttpResponseException("500");
    EraseRequestBody(ret);
    if (request_.GetBody().empty()) {
      close(write_fd_);
      SetEventStatus(WRITE_CLIENT);
    }
  }
}

void Client::SetPipe(int *pipe_write, int *pipe_read) {
  if (pipe(pipe_write) == -1) throw ft::HttpResponseException("500");
  if (pipe(pipe_read) == -1) throw ft::HttpResponseException("500");
}

void Client::ExecCGI(int *pipe_write, int *pipe_read, const CGI &cgi) {
  char **args = cgi.GetArgs();
  char **envs = cgi.GetEnvs();

  if (dup2(pipe_write[0], STDIN_FILENO) == -1)
    throw ft::HttpResponseException("500");
  if (dup2(pipe_read[1], STDOUT_FILENO) == -1)
    throw ft::HttpResponseException("500");

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

  if ((pid = fork()) < 0)
    throw ft::HttpResponseException("500");
  else if (pid == 0) {
    CGI cgi_vals = CGI(request_, port_, host_ip_, config_);
    ExecCGI(pipe_write, pipe_read, cgi_vals);
  }

  write_fd_ = pipe_write[1];
  if (fcntl(write_fd_, F_SETFL, O_NONBLOCK) != 0)
    throw ft::HttpResponseException("500");
  close(pipe_write[0]);

  read_fd_ = pipe_read[0];
  if (fcntl(write_fd_, F_SETFL, O_NONBLOCK) != 0)
    throw ft::HttpResponseException("500");
  close(pipe_read[1]);
}

// TODO
bool Client::IsValidRequest(void) {
  // if (request_.GetMethod() != GET) return false;
  return true;
}

void Client::HandleException(const char *err_msg) {
  response_.ErrorResponse(std::atoi(err_msg));
  SetEventStatus(WRITE_CLIENT);
}

std::string Client::MakePathUri(std::string alias_path,
                                std::string request_path,
                                std::string location_path) {
  if (alias_path.empty())
    // throw 404
    return "";

  std::vector<std::string> alias_splited = ft::vsplit(alias_path, '/');
  std::vector<std::string> request_splited = ft::vsplit(request_path, '/');
  std::vector<std::string> location_splited = ft::vsplit(location_path, '/');

  // step1: delta = request - location
  size_t i = 0;
  while (i < request_splited.size() &&
         request_splited[i] == location_splited[i])
    ++i;
  std::vector<std::string> delta(request_splited.size() - i);
  std::copy(request_splited.begin() + i, request_splited.end(), delta.begin());

  // step2: res = alias + delta
  std::vector<std::string> res_vector;
  for (size_t i = 0; i < alias_splited.size(); ++i)
    res_vector.push_back(alias_splited[i]);
  for (size_t i = 0; i < delta.size(); ++i) res_vector.push_back(delta[i]);

  // step3: make res
  std::string res;
  for (size_t i = 0; i < res_vector.size(); ++i) {
    res += res_vector[i];
    if (i != res_vector.size() - 1) res += "/";
  }

  return res;
}
