#include "Client.hpp"

const int Client::buf_max_ = 8192;

Client::Client(const std::map<std::string, config::Config> &config)
    : config_(config) {
  socket_status_ = READ_CLIENT;
  sended_ = 0;
}

int Client::ConnectClientSocket(int _fd) {
  struct sockaddr_storage clientaddr;
  socklen_t len = sizeof(clientaddr);
  int fd = accept(_fd, reinterpret_cast<SA *>(&clientaddr), &len);

  char hostname[buf_max_], port[buf_max_];
  getnameinfo(reinterpret_cast<SA *>(&clientaddr), len, hostname, buf_max_,
              port, buf_max_, 0);
  port_ = atoi(port);
  host_ip_ = std::string(hostname);

  if (fd == -1)
    throw std::runtime_error("accept: " + std::string(strerror(errno)));
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
  return fd;
}

int Client::RecvRequest(int client_fd) {
  int ret;
  char buf[buf_max_] = {0};

  ret = recv(client_fd, buf, buf_max_ - 1, 0);
  if (ret <= 0) return ret;

  try {
    request_.AppendRawData(buf, ret);
    if (request_.GetStatus() == HttpMessage::DONE) {
      // this is the first time to call of config getter
      // so this might throw 404 and that's OK!
      size_t max_body_size =
          GetConfig().GetClientMaxBodySize(request_.GetURI());

      if (max_body_size != 0 && max_body_size < request_.GetBody().size()) {
        throw ft::HttpResponseException("413");
      }
      Preprocess();
    }
  } catch (const Request::RequestFatalException &e) {
    std::cout << "RequestFatalException: " << e.what() << std::endl;
    throw ft::HttpResponseException("400");
  }
  return 1;
}

int Client::SendResponse(int client_fd) {
  int ret;

  size_t len = std::min((size_t)buf_max_, response_.Str().size() - sended_);
  ret = send(client_fd, &((response_.Str().c_str())[sended_]), len, 0);
  if (ret < 0) return ret;  // send error

  sended_ += ret;
  if (sended_ >= response_.Str().size()) {
    std::cout << LogMessage() << std::endl;
    response_.Clear();
    request_.Clear();
    sended_ = 0;
    socket_status_ = READ_CLIENT;
  }
  return 1;  // continue send
}

int Client::ReadStaticFile() {
  int ret = 1;
  char buf[buf_max_] = {0};

  ret = read(read_fd_, buf, buf_max_ - 1);
  if (ret == -1) {
    close(read_fd_);
    throw ft::HttpResponseException("500");
  } else if (ret == 0) {
    close(read_fd_);
    response_.AppendHeader("Content-Length",
                           ft::ltoa(response_.GetBody().size()));
    socket_status_ = WRITE_CLIENT;
  } else {
    std::vector<unsigned char> v(buf, buf + ret);
    response_.AppendBody(v);
  }
  return ret;
}

void Client::WriteStaticFile() {
  int ret = 1;

  size_t len = std::min((ssize_t)request_.GetBody().size(), (ssize_t)buf_max_);
  if (request_.GetMethod() == POST) {
    unsigned char *buf = reinterpret_cast<unsigned char *>(malloc(len));
    for (size_t i = 0; i < len; i++) {
      buf[i] = request_.GetBody()[i];
    }
    ret = write(write_fd_, buf, len);
    free(buf);
    if (ret < 0) throw ft::HttpResponseException("500");
  }

  request_.EraseBody(ret);
  if (request_.GetBody().empty()) {
    close(write_fd_);
    response_.SetStatusCode(201);
    response_.AppendHeader("Content-Type", "text/html");
    response_.AppendHeader("Content-Length", "0");

    socket_status_ = WRITE_CLIENT;
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
    throw ft::HttpResponseException("500");
  }
  response_.AppendRawData(buf, ret);
  if (ret == 0) {
    response_.EndCGI();
    waitpid(-1, NULL, 0);
    close(read_fd_);
    response_.SetStatusCode(200);
    response_.AppendHeader("Content-Length",
                           ft::ltoa(response_.GetBody().size()));
    socket_status_ = WRITE_CLIENT;
  }
  return ret;
}

void Client::WriteCGIin() {
  if (request_.GetMethod() == POST) {
    size_t len =
        std::min((ssize_t)request_.GetBody().size(), (ssize_t)buf_max_);
    int ret = 0;
    unsigned char *buf = reinterpret_cast<unsigned char *>(malloc(len));
    for (size_t i = 0; i < len; i++) {
      buf[i] = request_.GetBody()[i];
    }
    ret = write(write_fd_, buf, len);
    free(buf);
    if (ret < 0) throw ft::HttpResponseException("500");
    request_.EraseBody(ret);
  }
  if (request_.GetBody().empty()) {
    close(write_fd_);
    socket_status_ = READ_CGI;
  }
}

int Client::GetStatus() const { return socket_status_; }

int Client::GetWriteFd() const { return write_fd_; }

int Client::GetReadFd() const { return read_fd_; }

void Client::HandleException(const char *err_msg) {
  int status_code = std::atoi(err_msg);
  std::map<int, std::string> error_pages;
  try {
    try {  // when no location match url, 404 will be throwed by this function
      error_pages = GetConfig().GetErrorPages(request_.GetURI());
    } catch (const std::exception &e) {
      error_pages = GetConfig().GetMainAndServerErrorPages();
    }
    for (std::map<int, std::string>::iterator i = error_pages.begin();
         i != error_pages.end(); ++i) {
      if (status_code == i->first) {
        response_.SetStatusCode(status_code);
        if ((read_fd_ = open(i->second.c_str(), O_RDONLY)) < 0) break;
        if (fcntl(read_fd_, F_SETFL, O_NONBLOCK) == -1) break;
        socket_status_ = READ_FILE;
        return;
      }
    }
  } catch (ft::ConfigException &e) {
  }
  response_.ErrorResponse(status_code);
  socket_status_ = WRITE_CLIENT;
}

const config::Config &Client::GetConfig() const {
  return config_.find(request_.GetHeader("Host"))->second;
};

void Client::Preprocess(void) {
  std::string path_uri;
  std::string request_path = request_.GetURI();
  std::pair<int, std::string> redirect;

  socket_status_ = GetNextOfReadClient(path_uri, redirect);

  if (socket_status_ == READ_FILE) {
    if ((read_fd_ = open(path_uri.c_str(), O_RDONLY)) < 0)
      throw ft::HttpResponseException("403");
    if (fcntl(read_fd_, F_SETFL, O_NONBLOCK) == -1)
      throw ft::HttpResponseException("500");
  } else if (socket_status_ == WRITE_FILE) {
    if ((write_fd_ = open(path_uri.c_str(), O_RDWR | O_CREAT, 0644)) < 0)
      throw ft::HttpResponseException("403");
    if (fcntl(write_fd_, F_SETFL, O_NONBLOCK) < 0)
      throw ft::HttpResponseException("500");
  } else if (socket_status_ == READ_WRITE_CGI) {
    GenProcessForCGI(path_uri);
  } else if (socket_status_ == WRITE_CLIENT) {
    if (!(redirect.first == 0 && redirect.second.empty())) {  // redirect
      response_.RedirectResponse(redirect.first, redirect.second);
    } else if (request_.GetMethod() == DELETE) {  // DELETE
      response_.DeleteResponse();
    } else if (GetConfig().GetAutoindex(request_path)) {  // autoindex
      response_.AutoIndexResponse(path_uri.c_str(), request_path);
    } else
      throw ft::HttpResponseException("405");
  }
}

void Client::GenProcessForCGI(const std::string &path_uri) {
  int pipe_write[2];
  int pipe_read[2];
  pid_t pid;

  SetPipe(pipe_write, pipe_read);

  if ((pid = fork()) < 0) {
    close(pipe_write[0]);
    close(pipe_write[1]);
    close(pipe_read[0]);
    close(pipe_read[1]);
    throw ft::HttpResponseException("500");
  } else if (pid == 0) {
    CGI cgi_vals(request_, port_, host_ip_, GetConfig(), path_uri, path_info);
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

void Client::SetPipe(int *pipe_write, int *pipe_read) {
  if (pipe(pipe_write) == -1) throw ft::HttpResponseException("500");
  if (pipe(pipe_read) == -1) {
    close(pipe_write[0]);
    close(pipe_write[1]);
    throw ft::HttpResponseException("500");
  }
}

void Client::ExecCGI(int *pipe_write, int *pipe_read, CGI &cgi) {
  int ret1_dup2 = 0, ret2_dup2 = 0;

  ret1_dup2 = dup2(pipe_write[0], STDIN_FILENO);
  ret2_dup2 = dup2(pipe_read[1], STDOUT_FILENO);

  close(pipe_write[0]);
  close(pipe_write[1]);
  close(pipe_read[0]);
  close(pipe_read[1]);

  if (ret1_dup2 == -1 || ret2_dup2 == -1) exit(EXIT_FAILURE);

  cgi.Exec();
}

bool Client::IsValidExtension(std::string path_uri, std::string request_path) {
  size_t i = path_uri.length() - 1;

  while (i > 0 && path_uri[i] != '.') --i;
  std::string extension = path_uri.substr(i);

  std::set<std::string> allowd_extensions;
  allowd_extensions = GetConfig().GetExtensions(request_path);

  if (allowd_extensions.count(extension) == 1)
    return true;
  else
    return false;
}

bool Client::IsValidUploadRequest(const std::string &request_path) {
  std::string upload_pass = GetConfig().GetUploadPass(request_path);
  std::string upload_store = GetConfig().GetUploadStore(request_path);

  std::vector<std::string> request_splited = ft::vsplit(request_path, '/');
  std::vector<std::string> upload_pass_splited = ft::vsplit(upload_pass, '/');

  std::string upload_pass_full_path = MakePathUri(upload_pass, "/");
  std::string upload_store_full_path = MakePathUri(upload_store, "/");

  struct stat buffer;
  if (stat((upload_pass_full_path).c_str(), &buffer) == -1) {
    return false;
  }
  if (stat((upload_store_full_path).c_str(), &buffer) == -1) {
    return false;
  }
  if (upload_pass_splited.size() > request_splited.size()) {
    return false;
  }

  size_t i = 0;
  while (i < upload_pass_splited.size()) {
    if (request_splited[i] == upload_pass_splited[i]) {
      ++i;
    } else {
      break;
    }
  }

  return i == upload_pass_splited.size();
}

bool Client::DirectoryRedirect(std::string request_path) {
  std::string location_path = GetConfig().GetPath(request_path);
  std::string path_uri = MakePathUri(request_path, location_path);

  struct stat buffer;
  stat((path_uri).c_str(), &buffer);

  if (S_ISREG(buffer.st_mode) && request_path[request_path.size() - 1] == '/') {
    throw ft::HttpResponseException("404");
  }

  if (S_ISDIR(buffer.st_mode) && request_path[request_path.size() - 1] != '/')
    return true;
  else
    return false;
}

std::string Client::GetIndexFileIfExist(std::string path_uri,
                                        std::string request_path) {
  std::vector<std::string> conf_indexes = GetConfig().GetIndexes(request_path);

  for (size_t i = 0; i < conf_indexes.size(); ++i) {
    struct stat buffer;
    std::string filename = conf_indexes[i];

    if (stat((path_uri + filename).c_str(), &buffer) != -1 &&
        S_ISREG(buffer.st_mode)) {
      return filename;
    }
  }
  return "";
}

enum SocketStatus Client::GetNextOfReadClient(
    std::string &path_uri, std::pair<int, std::string> &redirect) {
  std::string request_path = request_.GetURI();
  std::string location_path = GetConfig().GetPath(request_path);
  enum Method method = request_.GetMethod();

  try {  // checking HTTP method is allowed and location directive exists
    if (GetConfig()
            .GetAllowedMethods(request_path)
            .count(request_.GetMethod()) == 0) {
      throw ft::HttpResponseException("405");
    }
  } catch (const ft::ConfigException &e) {
    throw ft::HttpResponseException("404");
  }

  redirect = GetConfig().GetRedirect(request_path);
  if (redirect.first == 0 && redirect.second.empty() &&
      DirectoryRedirect(request_path)) {
    redirect.first = 301;
    std::string host_name;
    try {
      host_name = request_.GetHeader("Host");
    } catch (const HttpMessage::HeaderKeyException &e) {
      host_name = GetConfig().GetHost();
    }

    redirect.second = "http://" + host_name + ":" +
                      ft::ltoa(GetConfig().GetPort()) + request_path + "/";
  }
  if (redirect.first != 0 || redirect.second.empty() == false)
    return WRITE_CLIENT;

  enum SocketStatus ret = WRITE_CLIENT;

  path_uri = MakePathUri(request_path, location_path);

  switch (method) {
    case GET:
      ret = HandleGET(path_uri);
      break;
    case POST:
      ret = HandlePOST(path_uri);
      break;
    case DELETE:
      ret = HandleDELETE(path_uri);
      break;
    default:
      break;
  }

  return ret;
}

std::string Client::MakePathUri(std::string request_path,
                                std::string location_path) {
  std::string alias_path = GetConfig().GetAlias(request_path);

  if (alias_path.empty()) {
    throw ft::HttpResponseException("404");
  }

  std::vector<std::string> alias_splited = ft::vsplit(alias_path, '/');
  std::vector<std::string> request_splited = ft::vsplit(request_path, '/');
  std::vector<std::string> location_splited = ft::vsplit(location_path, '/');

  // step1: delta = request - location
  size_t i = 0;
  while (i < location_splited.size() &&
         request_splited[i] == location_splited[i])
    ++i;
  std::vector<std::string> delta(request_splited.size() - i);
  std::copy(request_splited.begin() + i, request_splited.end(), delta.begin());

  // step2: res = alias + delta
  std::vector<std::string> res_vector;
  for (size_t i = 0; i < alias_splited.size(); ++i) {
    res_vector.push_back(alias_splited[i]);
  }
  for (size_t i = 0; i < delta.size(); ++i) {
    res_vector.push_back(delta[i]);
  }

  // step3: make res
  std::string res;
  for (size_t i = 0; i < res_vector.size(); ++i) {
    res += res_vector[i];
    if (i != res_vector.size() - 1) res += "/";
  }

  return res;
}

enum SocketStatus Client::HandleGET(std::string &path_uri) {
  struct stat buffer;
  if (stat((path_uri).c_str(), &buffer) == -1) {
    throw ft::HttpResponseException("404");
  }

  std::string request_path = request_.GetURI();

  // (UPLOAD) by GET is 405
  if (!GetConfig().GetUploadPass(request_path).empty() &&
      !GetConfig().GetUploadStore(request_path).empty()) {
    if (IsValidUploadRequest(request_path)) {
      throw ft::HttpResponseException("405");
    }
  }

  // (CGI)
  if (IsValidExtension(path_uri, request_path) && S_ISREG(buffer.st_mode)) {
    path_info = request_path;
    return READ_WRITE_CGI;
  }

  // (SIMPLE GET)
  if (S_ISREG(buffer.st_mode)) {
    return READ_FILE;
  } else if (S_ISDIR(buffer.st_mode)) {
    path_uri += "/";

    std::string index_filename = GetIndexFileIfExist(path_uri, request_path);
    if (index_filename.empty() == false) {
      path_uri += index_filename;

      // (CGI)
      if (IsValidExtension(path_uri, request_path)) {
        path_info = request_path + index_filename;
        return READ_WRITE_CGI;
      }

      // (GET INDEX)
      return READ_FILE;
    } else if (GetConfig().GetAutoindex(request_path) == true) {
      // (AUTOINDEX)
      return WRITE_CLIENT;
    }
  }

  throw ft::HttpResponseException("404");
}

enum SocketStatus Client::HandlePOST(std::string &path_uri) {
  std::string request_path = request_.GetURI();

  // (UPLOAD)
  if (!GetConfig().GetUploadPass(request_path).empty() &&
      !GetConfig().GetUploadStore(request_path).empty()) {
    if (IsValidUploadRequest(request_path)) {
      std::string filename = std::string("/" + ft::what_time());
      std::string store = GetConfig().GetUploadStore(request_path);
      if (store[store.size() - 1] == '/')
        store = store.substr(0, store.size() - 1);
      store = store.empty() ? "/" : store;
      path_uri = MakePathUri(store, "/") + filename;
      response_.AppendHeader("Content-Location", store + filename);
      return WRITE_FILE;
    }
  }
  struct stat buffer;
  if (stat((path_uri).c_str(), &buffer) == -1) {
    throw ft::HttpResponseException("404");
  }

  // (CGI)
  if (IsValidExtension(path_uri, request_path) && S_ISREG(buffer.st_mode)) {
    path_info = request_path;
    return READ_WRITE_CGI;
  }

  throw ft::HttpResponseException("405");
}

enum SocketStatus Client::HandleDELETE(std::string &path_uri) {
  struct stat buffer;
  if (stat((path_uri).c_str(), &buffer) == -1) {
    throw ft::HttpResponseException("404");
  }

  std::string request_path = request_.GetURI();

  // (UPLOAD) by GET is 405
  if (!GetConfig().GetUploadPass(request_path).empty() &&
      !GetConfig().GetUploadStore(request_path).empty()) {
    if (IsValidUploadRequest(request_path)) {
      throw ft::HttpResponseException("405");
    }
  }

  remove((path_uri).c_str());
  return WRITE_CLIENT;
}

std::string Client::LogMessage() const {
  std::string method_array[4] = {"GET", "POST", "DELETE", "INVALID"};
  std::stringstream ss;
  ss << host_ip_;  // << ":" << port_;
  ss << " - [" << response_.GetHeader("Date") << "] ";
  ss << "\"" << method_array[request_.GetMethod()] << " ";
  ss << request_.GetURI() << " ";
  ss << "HTTP/" << request_.GetVersion() << "\" ";
  ss << response_.GetStatusCode() << " ";
  try {
    ss << response_.GetHeader("Content-Length");
    ss << " \"" << request_.GetHeader("User-Agent") << "\"";
  } catch (std::exception &e) {
  }
  return ss.str();
}
