#include "WebServ.hpp"

#include <unistd.h>

#include "Client.hpp"
#include "Server.hpp"

const std::string WebServ::default_path = "./conf/default.conf";
const int WebServ::buf_max = 8192;

WebServ::WebServ(const std::string &path) {
  timeout = (struct timeval){1, 0};

  ParseConig(path);
}

WebServ::~WebServ() {
  // delete all pointers
  for (std::map<int, Socket *>::iterator it = sockets.begin();
       it != sockets.end(); ++it) {
    delete it->second;
  }
  sockets.clear();
}

void WebServ::ParseConig(const std::string &path) {
  // TODO: parse config fully
  (void)path;

  // コンフィグをパースした結果分かる、最初に立てるべきサーバーたちをつくる
  for (int i = 0; i < 3; ++i) {
    Server *server = new Server(4200 + i, "127.0.0.1");
    long fd = server->SetSocket(42);  // 42 is meanless

    sockets[fd] = server;
  }
}

int WebServ::AcceptSession(socket_iter it) {
  int accepted = -1;
  int server_fd = it->first;

  // サーバーソケットでISSETがtrueなのはnew clientの1パターンしかない
  // これはサーバーソケットならば常に可能性があるから、enum管理不要
  if (FD_ISSET(server_fd, &rfd_set)) {
    Client *client = new Client();

    int client_fd = client->SetSocket(server_fd);

    if (client_fd > max_fd) max_fd = client_fd;
    sockets[client_fd] = client;

    accepted = 1;
  }
  return accepted;
}

int WebServ::ReadClient(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);

  int ret = client->recv(client_fd);

  switch (ret) {
    case -1:
      close(client_fd);
      delete it->second;
      sockets.erase(it);
      throw std::runtime_error("recv error\n");
      break;

    case 0:
      close(client_fd);
      delete it->second;
      sockets.erase(it);
      break;

    default:
      client->Prepare();
      break;
  }

  return ret;
}

int WebServ::ReadFile(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret = 1;
  char buf[WebServ::buf_max] = {0};

  read(client->GetReadFd(), buf, Client::buf_max - 1);

  close(client->GetReadFd());

  client->GetResponse().SetBody(buf);
  client->SetStatus(WRITE_CLIENT);

  return ret;
}

int WebServ::WriteFile(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret = 1;

  write(client->GetWriteFd(), "<p>This is post.</p>", 20);
  close(client->GetWriteFd());
  client->SetStatus(WRITE_CLIENT);

  return ret;
}

int WebServ::ReadCGI(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret = 1;
  char buf[WebServ::buf_max] = {0};

  read(client->GetReadFd(), buf, Client::buf_max - 1);

  close(client->GetReadFd());

  client->GetResponse().SetBody(buf);
  client->SetStatus(WRITE_CLIENT);

  return ret;
}

int WebServ::WriteCGI(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret = 1;

  write(client->GetWriteFd(), "", 0);
  close(client->GetWriteFd());
  client->SetStatus(READ_CGI);

  return ret;
}

int WebServ::WriteClient(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret = -1;

  client->GetResponse().AppendHeader(
      "Content-Length", ft::ltoa(client->GetResponse().GetBody().length()));

  // 完成したレスポンスを送る
  ret = client->send(client_fd);

  if (ret == -1) {
    close(client_fd);
    sockets.erase(it);
    throw std::runtime_error("send error\n");
  } else {
    // TODO: can we send all data by one send(2)?
    // 次のrequestを待つ
    client->SetStatus(READ_CLIENT);
    ret = 1;
  }

  return ret;
}

int WebServ::HasUsableIO() {
  int n = 0;

  while (n == 0) {
    FD_ZERO(&rfd_set);
    FD_ZERO(&wfd_set);
    max_fd = 0;

    for (std::map<int, Socket *>::iterator it = sockets.begin();
         it != sockets.end(); ++it) {
      // set listening sockets
      if (dynamic_cast<Server *>(it->second)) {
        int serevr_fd = it->first;

        FD_SET(serevr_fd, &rfd_set);
        max_fd = std::max(max_fd, serevr_fd);
      } else {
        int client_fd = it->first;
        Client *client = dynamic_cast<Client *>(sockets[client_fd]);

        switch (client->GetStatus()) {
          case READ_CLIENT:
            FD_SET(client_fd, &rfd_set);
            max_fd = std::max(max_fd, client_fd);
            break;

          case WRITE_CLIENT:
            FD_SET(client_fd, &wfd_set);
            max_fd = std::max(max_fd, client_fd);
            break;

          case READ_FILE:
            FD_SET(client->GetReadFd(), &rfd_set);
            max_fd = std::max(max_fd, client->GetReadFd());
            break;

          case WRITE_FILE:
            FD_SET(client->GetWriteFd(), &wfd_set);
            max_fd = std::max(max_fd, client->GetWriteFd());
            break;

          case READ_CGI:
            FD_SET(client->GetReadFd(), &rfd_set);
            max_fd = std::max(max_fd, client->GetReadFd());
            break;

          case WRITE_CGI:
            FD_SET(client->GetWriteFd(), &wfd_set);
            max_fd = std::max(max_fd, client->GetWriteFd());
            break;
        }
      }
    }

    n = select(max_fd + 1, &rfd_set, &wfd_set, NULL, &timeout);
  }

  return n;
}

void WebServ::ExecClientEvent(socket_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);

  switch (client->GetStatus()) {
    case READ_CLIENT:
      if (FD_ISSET(client_fd, &rfd_set)) {
        ReadClient(it);
      }
      break;

    case READ_FILE:
      if (FD_ISSET(client->GetReadFd(), &rfd_set)) {
        ReadFile(it);
      }
      break;

    case WRITE_FILE:
      if (FD_ISSET(client->GetWriteFd(), &wfd_set)) {
        WriteFile(it);
      }
      break;

    case WRITE_CGI:
      if (FD_ISSET(client->GetWriteFd(), &wfd_set)) {
        WriteCGI(it);
      }
      break;

    case READ_CGI:
      if (FD_ISSET(client->GetReadFd(), &rfd_set)) {
        ReadCGI(it);
      }
      break;

    case WRITE_CLIENT:
      if (FD_ISSET(client_fd, &wfd_set)) {
        WriteClient(it);
      }
      break;
  }
}

void WebServ::Activate(void) {
  while (true) {
    int n = HasUsableIO();  // polling I/O

    if (n > 0) {
      for (std::map<int, Socket *>::iterator it = sockets.begin();
           it != sockets.end(); ++it) {
        if (dynamic_cast<Server *>(it->second)) {
          AcceptSession(it);
        } else {
          ExecClientEvent(it);
        }
      }
    } else {
      throw std::runtime_error("select error\n");
    }
  }
}
