#include "WebServ.hpp"

#include <unistd.h>

#include <cstring>
#include <iostream>

#include "Client.hpp"
#include "Server.hpp"

const std::string WebServ::default_path_ = "./conf/default.conf";
const int WebServ::buf_max_ = 8192;

WebServ::WebServ(const std::string &path) {
  timeout_ = (struct timeval){1, 0};

  ParseConig(path);
}

WebServ::~WebServ() {
  // delete all pointers
  for (std::map<int, Socket *>::iterator it = sockets_.begin();
       it != sockets_.end(); ++it) {
    delete it->second;
  }
  sockets_.clear();
}

void WebServ::ParseConig(const std::string &path) {
  // TODO: parse config fully
  (void)path;

  // コンフィグをパースした結果分かる、最初に立てるべきサーバーたちをつくる
  for (int i = 0; i < 3; ++i) {
    Server *server = new Server(4200 + i, "127.0.0.1");
    long fd = server->SetSocket(42);  // 42 is meanless

    sockets_[fd] = server;
  }
}

int WebServ::AcceptSession(socket_iter it) {
  int accepted = -1;
  int server_fd = it->first;

  if (FD_ISSET(server_fd, &rfd_set_)) {
    Client *client = new Client();

    int client_fd = client->SetSocket(server_fd);

    if (client_fd > max_fd_) max_fd_ = client_fd;
    sockets_[client_fd] = client;

    accepted = 1;
  }
  return accepted;
}

int WebServ::ReadClient(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);

  int ret = client->recv(client_fd);

  switch (ret) {
    case -1:  // recv error
      close(client_fd);
      delete it->second;
      sockets_.erase(it);
      throw std::runtime_error("recv error\n");
      break;

    case 0:  // closed by client
      close(client_fd);
      delete it->second;
      sockets_.erase(it);
      break;

    case 1:  // read complete
      client->Prepare();
      break;

    case 2:  // need to read more
      break;
  }

  return ret;
}

int WebServ::ReadFile(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret = 1;
  char buf[WebServ::buf_max_] = {0};

  ret = read(client->GetReadFd(), buf, WebServ::buf_max_ - 1);

  switch (ret) {
    case -1:  // read error
      close(client->GetReadFd());
      close(client_fd);
      delete it->second;
      sockets_.erase(it);
      throw std::runtime_error("read error\n");
      break;

    case 0:  // read complete
      close(client->GetReadFd());
      client->SetStatus(WRITE_CLIENT);
      break;

    default:  // just read
      client->AppendResponseBody(buf);
      break;
  }

  return ret;
}

int WebServ::WriteFile(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret = 1;

  write(client->GetWriteFd(), "<p>This is post.</p>", 20);

  close(client->GetWriteFd());
  client->SetStatus(WRITE_CLIENT);

  return ret;
}

// parse CGI output i.e. set body in response_ properly
void WebServ::ParseCGIOutput(std::string headers, std::string body,
                             Client *client) {
  std::vector<std::string> all = ft::vsplit(headers, '\n');
  int content_length_exist = 0;
  int chunked_exist = 0;

  // append headers, checking if content-length exist and chunked exist
  for (std::string::size_type i = 0; i < all.size(); i++) {
    std::pair<std::string, std::string> header = ft::div(all[i], ':');

    if (ft::strcasecmp(header.first, "Content-Length") == 0) {
      content_length_exist = 1;
      continue;  // content-length is set in WriteClient()
    } else if (ft::strcasecmp(header.first, "Transfer-Encoding") == 0) {
      if (strcmp(ft::trim(header.second).c_str(), "chunked") != 0)
        throw std::runtime_error("invalid transfer-encoding\n");
      else {
        chunked_exist = 1;
        continue;  // content-length is set in WriteClient()
      }
    }
    client->AppendResponseHeader(header);
  }

  if (content_length_exist) {
    client->AppendResponseBody(body);
  } else if (!chunked_exist) {
    client->AppendResponseBody(body);
  } else {  // chunked
    std::string::size_type length = 0;
    while (body.find("\r\n") != std::string::npos) {
      std::string::size_type len = strtoul(body.c_str(), NULL, 16);
      std::string data = body.substr(body.find("\r\n") + 2);
      if (data.size() >= len) {

        client->AppendResponseBody(data.substr(0, len));
        body = body.substr(body.find("\r\n") + 2 + len + 2);
      }
      if (len == 0) break;
      length += len;
    }
  }
}

int WebServ::ReadCGI(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret = 1;
  char buf[10] = {0};
  std::string headers;
  std::string body;

  ret = read(client->GetReadFd(), buf, 10 - 1);

  switch (ret) {
    case -1:  // read error
      close(client->GetReadFd());
      close(client_fd);
      delete it->second;
      sockets_.erase(it);
      throw std::runtime_error("read error\n");
      break;

    case 0:  // read complete
      close(client->GetReadFd());
      if (cgi_outputs_[client->GetReadFd()].find("\n\n") == std::string::npos)
        throw std::runtime_error("incomplete header\n");

      headers = cgi_outputs_[client->GetReadFd()].substr(
          0, cgi_outputs_[client->GetReadFd()].find("\n\n"));
      body = cgi_outputs_[client->GetReadFd()].substr(
          cgi_outputs_[client->GetReadFd()].find("\n\n") + 2);

      ParseCGIOutput(headers, body, client);

      client->SetStatus(WRITE_CLIENT);
      cgi_outputs_.erase(client->GetReadFd());
      break;

    default:  // just read
      cgi_outputs_[client->GetReadFd()].append(buf);
      break;
  }
  // client->SetStatus(WRITE_CLIENT);

  return ret;
}

int WebServ::WriteCGI(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret = 1;

  write(client->GetWriteFd(), "", 0);

  close(client->GetWriteFd());
  client->SetStatus(READ_CGI);

  return ret;
}

int WebServ::WriteClient(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret = -1;

  client->AppendResponseHeader(
      "Content-Length", ft::ltoa(client->GetResponse().GetBody().length()));

  // 完成したレスポンスを送る
  ret = client->send(client_fd);

  if (ret == -1) {
    close(client_fd);
    sockets_.erase(it);
    throw std::runtime_error("send error\n");
  }
  // TODO: can we send all data by one send(2)?
  // 次のrequest_を待つ
  client->SetStatus(READ_CLIENT);
  ret = 1;

  return ret;
}

int WebServ::HasUsableIO() {
  int n = 0;

  while (n == 0) {
    FD_ZERO(&rfd_set_);
    FD_ZERO(&wfd_set_);
    max_fd_ = 0;

    for (std::map<int, Socket *>::iterator it = sockets_.begin();
         it != sockets_.end(); ++it) {
      // set listening sockets_
      if (dynamic_cast<Server *>(it->second)) {
        int serevr_fd = it->first;

        FD_SET(serevr_fd, &rfd_set_);
        max_fd_ = std::max(max_fd_, serevr_fd);
      } else {
        int client_fd = it->first;
        Client *client = dynamic_cast<Client *>(sockets_[client_fd]);

        switch (client->GetStatus()) {
          case READ_CLIENT:
            FD_SET(client_fd, &rfd_set_);
            max_fd_ = std::max(max_fd_, client_fd);
            break;

          case WRITE_CLIENT:
            FD_SET(client_fd, &wfd_set_);
            max_fd_ = std::max(max_fd_, client_fd);
            break;

          case READ_FILE:
            FD_SET(client->GetReadFd(), &rfd_set_);
            max_fd_ = std::max(max_fd_, client->GetReadFd());
            break;

          case WRITE_FILE:
            FD_SET(client->GetWriteFd(), &wfd_set_);
            max_fd_ = std::max(max_fd_, client->GetWriteFd());
            break;

          case READ_CGI:
            FD_SET(client->GetReadFd(), &rfd_set_);
            max_fd_ = std::max(max_fd_, client->GetReadFd());
            break;

          case WRITE_CGI:
            FD_SET(client->GetWriteFd(), &wfd_set_);
            max_fd_ = std::max(max_fd_, client->GetWriteFd());
            break;
        }
      }
    }

    n = select(max_fd_ + 1, &rfd_set_, &wfd_set_, NULL, &timeout_);
  }

  return n;
}

int WebServ::ExecClientEvent(socket_iter it) {
  int hit_flag = 0;
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);

  switch (client->GetStatus()) {
    case READ_CLIENT:
      if (FD_ISSET(client_fd, &rfd_set_)) {
        ReadClient(it);
        hit_flag = 1;
      }
      break;

    case READ_FILE:
      if (FD_ISSET(client->GetReadFd(), &rfd_set_)) {
        ReadFile(it);
        hit_flag = 1;
      }
      break;

    case WRITE_FILE:
      if (FD_ISSET(client->GetWriteFd(), &wfd_set_)) {
        WriteFile(it);
        hit_flag = 1;
      }
      break;

    case WRITE_CGI:
      if (FD_ISSET(client->GetWriteFd(), &wfd_set_)) {
        WriteCGI(it);
        hit_flag = 1;
      }
      break;

    case READ_CGI:
      if (FD_ISSET(client->GetReadFd(), &rfd_set_)) {
        ReadCGI(it);
        hit_flag = 1;
      }
      break;

    case WRITE_CLIENT:
      if (FD_ISSET(client_fd, &wfd_set_)) {
        WriteClient(it);
        hit_flag = 1;
      }
      break;
  }
  return hit_flag;
}

void WebServ::Activate(void) {
  while (true) {
    int n = HasUsableIO();

    if (n > 0) {
      for (std::map<int, Socket *>::iterator it = sockets_.begin();
           n && it != sockets_.end(); ++it) {
        if (dynamic_cast<Server *>(it->second)) {
          if (AcceptSession(it) == 1) --n;
        } else {
          if (ExecClientEvent(it) == 1) --n;
        }
      }
    } else {
      throw std::runtime_error("select error\n");
    }
  }
}
