#include "WebServ.hpp"

#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "Client.hpp"
#include "Server.hpp"
#include "config.hpp"

const std::string WebServ::default_path_ = "./conf/default.conf";

WebServ::WebServ(const std::string &path) {
  timeout_ = (struct timeval){1, 0};

  ParseConfig(path);
}

WebServ::~WebServ() {
  // delete all pointers
  for (std::map<int, Socket *>::iterator it = sockets_.begin();
       it != sockets_.end(); ++it) {
    delete it->second;
  }
  sockets_.clear();
}

void WebServ::ParseConfig(const std::string &path) {
  config::Parser parser(path);

  std::vector<config::Config> configs = parser.GetConfigs();
  if (configs.empty()) return;

  std::vector<config::Config>::const_iterator itr;
  for (itr = configs.begin(); itr != configs.end(); ++itr) {
    Server *server = new Server(*itr);
    int fd = server->SetSocket();
    sockets_[fd] = server;
  }
}

int WebServ::AcceptSession(socket_iter it) {
  int accepted = -1;
  int server_fd = it->first;

  if (FD_ISSET(server_fd, &rfd_set_)) {
    Server *server = dynamic_cast<Server *>(it->second);
    Client *client = new Client(server->GetConfig());

    int client_fd = client->SetSocket(server_fd);

    if (client_fd > max_fd_) max_fd_ = client_fd;
    sockets_[client_fd] = client;

    accepted = 1;
  }
  return accepted;
}

void WebServ::ReadClient(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret;

  if ((ret = client->RecvRequest(client_fd)) <= 0) {
    close(client_fd);
    delete it->second;
    sockets_.erase(it);
    if (ret < 0) throw std::runtime_error(strerror(errno));
  }
}

void WebServ::ReadFile(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret;

  if ((ret = client->ReadStaticFile()) < 0) {
    close(client_fd);
    delete it->second;
    sockets_.erase(it);
    throw std::runtime_error(strerror(errno));
  }
}

void WebServ::ReadCGI(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret;

  if ((ret = client->ReadCGIout()) < 0) {
    close(client_fd);
    delete it->second;
    sockets_.erase(it);
    throw std::runtime_error(strerror(errno));
  }
}

void WebServ::WriteClient(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  int ret;

  if ((ret = client->SendResponse(client_fd)) < 0) {
    close(client_fd);
    sockets_.erase(it);
    throw std::runtime_error(strerror(errno));
  }
}

void WebServ::WriteFile(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  client->WriteStaticFile();
}

void WebServ::WriteCGI(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);
  client->WriteCGIin();
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

          case READ_WRITE_CGI:
            FD_SET(client->GetReadFd(), &rfd_set_);
            FD_SET(client->GetWriteFd(), &wfd_set_);
            max_fd_ = std::max(
                max_fd_, std::max(client->GetWriteFd(), client->GetReadFd()));
            break;
        }
      }
    }

    n = select(max_fd_ + 1, &rfd_set_, &wfd_set_, NULL, &timeout_);
  }

  return n;
}

int WebServ::ExecClientEvent(socket_iter it) {
  int hit = 0;
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets_[client_fd]);

  if (client->GetStatus() == READ_CLIENT && FD_ISSET(client_fd, &rfd_set_)) {
    ReadClient(it);
    ++hit;
  }

  if (client->GetStatus() == READ_FILE &&
      FD_ISSET(client->GetReadFd(), &rfd_set_)) {
    ReadFile(it);
    ++hit;
  }

  if (client->GetStatus() == WRITE_FILE &&
      FD_ISSET(client->GetWriteFd(), &wfd_set_)) {
    WriteFile(it);
    ++hit;
  }

  if (client->GetStatus() == READ_WRITE_CGI &&
      FD_ISSET(client->GetWriteFd(), &wfd_set_)) {
    WriteCGI(it);
    ++hit;
  }

  if ((client->GetStatus() == READ_CGI ||
       client->GetStatus() == READ_WRITE_CGI) &&
      FD_ISSET(client->GetReadFd(), &rfd_set_)) {
    ReadCGI(it);
    ++hit;
  }

  if (client->GetStatus() == WRITE_CLIENT && FD_ISSET(client_fd, &wfd_set_)) {
    WriteClient(it);
    ++hit;
  }

  return hit;
}

void WebServ::Activate(void) {
  int hit = 0;

  while (true) {
    int n = HasUsableIO();

    if (n > 0) {
      for (std::map<int, Socket *>::iterator it = sockets_.begin();
           n && it != sockets_.end(); ++it) {
        try {
          if (dynamic_cast<Server *>(it->second)) {
            if (AcceptSession(it) == 1) --n;
          } else {
            if ((hit = ExecClientEvent(it)) > 0) n -= hit;
          }
        } catch (ft::HttpResponseException &e) {
          Client *client = dynamic_cast<Client *>(it->second);
          client->HandleException(e.what());
        }
      }
    } else {
      throw std::runtime_error("select error\n");
    }
  }
}
