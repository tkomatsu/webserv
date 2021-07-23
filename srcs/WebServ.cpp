#include "WebServ.hpp"

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

int WebServ::AcceptSession(map_iter it) {
  int accepted = 0;
  int server_fd = it->first;

  // サーバーソケットでISSETがtrueなのはnew clientの1パターンしかない
  // これはサーバーソケットならば常に可能性があるから、enum管理不要
  if (FD_ISSET(server_fd, &rfd_set)) {
    Client *client = new Client();
    client->SetStatus(READ_CLIENT);
    int client_fd = client->SetSocket(server_fd);

    if (client_fd > max_fd) max_fd = client_fd;
    sockets[client_fd] = client;

    accepted = 1;
  }
  return accepted;
}

int WebServ::ReadClient(map_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);

  int ret = client->recv(client_fd);

  if (ret == -1) {
    close(client_fd);
    delete it->second;
    sockets.erase(it);
    throw std::runtime_error("recv error\n");
  } else if (ret == 0) {
    close(client_fd);
    delete it->second;
    sockets.erase(it);
  } else {
    client->Prepare();
  }

  return ret;
}

int WebServ::ReadFile(map_iter it) {
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

int WebServ::WriteFile(map_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret = 1;

  write(client->GetWriteFd(), "<p>This is post.</p>", 20);
  close(client->GetWriteFd());
  client->SetStatus(WRITE_CLIENT);

  return ret;
}

int WebServ::ReadCGI(map_iter it) {
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

int WebServ::WriteCGI(map_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret = 1;

  write(client->GetWriteFd(), "", 0);
  close(client->GetWriteFd());
  client->SetStatus(READ_CGI);

  return ret;
}

int WebServ::WriteClient(map_iter it) {
  int client_fd = it->first;
  Client *client = dynamic_cast<Client *>(sockets[client_fd]);
  int ret;

  client->GetResponse().AppendHeader(
      "Content-Length", ft_ltoa(client->GetResponse().GetBody().length()));

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
      } else if (dynamic_cast<Client *>(it->second)) {
        int client_fd = it->first;
        Client *client = dynamic_cast<Client *>(sockets[client_fd]);

        if (client->GetStatus() == READ_CLIENT) {
          FD_SET(client_fd, &rfd_set);
          max_fd = std::max(max_fd, client_fd);
        } else if (client->GetStatus() == WRITE_CLIENT) {
          FD_SET(client_fd, &wfd_set);
          max_fd = std::max(max_fd, client_fd);
        } else if (client->GetStatus() == READ_FILE) {
          FD_SET(client->GetReadFd(), &rfd_set);
          max_fd = std::max(max_fd, client->GetReadFd());
        } else if (client->GetStatus() == WRITE_FILE) {
          FD_SET(client->GetWriteFd(), &wfd_set);
          max_fd = std::max(max_fd, client->GetWriteFd());
        } else if (client->GetStatus() == READ_CGI) {
          FD_SET(client->GetReadFd(), &rfd_set);
          max_fd = std::max(max_fd, client->GetReadFd());
        } else if (client->GetStatus() == WRITE_CGI) {
          FD_SET(client->GetWriteFd(), &wfd_set);
          max_fd = std::max(max_fd, client->GetWriteFd());
        }
      }
    }

    n = select(max_fd + 1, &rfd_set, &wfd_set, NULL, &timeout);
  }

  return n;
}

void WebServ::Activate(void) {
  while (true) {
    int n = HasUsableIO();  // polling I/O

    if (n > 0) {
      for (std::map<int, Socket *>::iterator it = sockets.begin();
           it != sockets.end(); ++it) {
        // サーバーソケット
        if (dynamic_cast<Server *>(it->second)) {
          if (AcceptSession(it)) break;
        }

        // クライアントソケット
        if (dynamic_cast<Client *>(it->second)) {
          int client_fd = it->first;
          Client *client = dynamic_cast<Client *>(sockets[client_fd]);

          // パターン１：クライアントからrecvする (最初はみんなこれ)
          if (client->GetStatus() == READ_CLIENT &&
              FD_ISSET(client_fd, &rfd_set)) {
            if (ReadClient(it)) break;
          }

          // パターン２：特定のファイルをreadする
          if (client->GetStatus() == READ_FILE &&
              FD_ISSET(client->GetReadFd(),
                       &rfd_set)) {  // client_fdではない何か
            if (ReadFile(it)) break;
          }

          // パターン３：特定のファイルにwriteする
          if (client->GetStatus() == WRITE_FILE &&
              FD_ISSET(client->GetWriteFd(),
                       &wfd_set)) {  // client_fdではない何か
            if (WriteFile(it)) break;
          }

          // パターン４：CGIにこっちの標準入力をwriteする
          if (client->GetStatus() == WRITE_CGI &&
              FD_ISSET(client->GetWriteFd(), &wfd_set)) {
            if (WriteCGI(it)) break;
          }

          // パターン５：CGIの標準出力をreadする（パターン４の次に来るところ）
          if (client->GetStatus() == READ_CGI &&
              FD_ISSET(client->GetReadFd(), &rfd_set)) {
            if (ReadCGI(it)) break;
          }

          // パターン６：クライアントにsendする(最後みんなこれ)
          if (client->GetStatus() == WRITE_CLIENT &&
              FD_ISSET(client_fd, &wfd_set)) {
            if (WriteClient(it)) break;
          }
        }
      }
    } else {
      throw std::runtime_error("select error\n");
    }
  }
}
