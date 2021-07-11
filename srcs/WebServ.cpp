#include "WebServ.hpp"

const std::string WebServ::default_path = "./conf/default.conf";

WebServ::WebServ(const std::string &path) {
  max_fd = 0;
  FD_ZERO(&master_set);

  parseConfig(path);
}

WebServ::~WebServ() {
  // delete all pointers
  for (std::map<long, ISocket *>::iterator it = sockets.begin();
       it != sockets.end(); ++it) {
    delete it->second;
  }
  sockets.clear();
}

void WebServ::parseConfig(const std::string &path) {
  // TODO: parse config fully
  (void)path;

  for (int i = 0; i < 3; ++i) {
    Server *server = new Server(4200 + i, "127.0.0.1");
    long fd = server->makeSocket(42);  // 42 is meanless

    sockets[fd] = server;
  }
}

void WebServ::start(void) {
  // set listening sockets
  for (std::map<long, ISocket *>::iterator it = sockets.begin();
       it != sockets.end(); ++it) {
    long server_fd = it->first;

    FD_SET(it->first, &master_set);
    if (server_fd > max_fd) max_fd = server_fd;
  }
  if (max_fd == 0) throw std::runtime_error("no server error\n");

  while (true) {
    int n = 0;
    fd_set rfd_set, wfd_set;
    struct timeval timeout = (struct timeval){1, 0};

    // selecting readables or writables
    while (n == 0) {
      memcpy(&rfd_set, &master_set, sizeof(master_set));
      FD_ZERO(&wfd_set);
      for (std::map<long, ISocket *>::iterator it = sockets.begin();
           it != sockets.end(); ++it)
        if (dynamic_cast<Client *>(it->second)) {
          int client_fd = it->first;
          Client *client = dynamic_cast<Client *>(sockets[client_fd]);

          if (client->socket_status == WRITE_CLIENT ||
              client->socket_status == WRITE_FILE ||
              client->socket_status == WRITE_CGI)
            FD_SET(client_fd, &wfd_set);
        }

      n = select(max_fd + 1, &rfd_set, &wfd_set, NULL, &timeout);
    }

    if (n > 0) {
      for (std::map<long, ISocket *>::iterator it = sockets.begin();
           it != sockets.end(); ++it) {
        // サーバーソケットでISSETがtrueなのはnew clientの1パターンしかない
        // これはサーバーソケットならば常に可能性があるから、enum管理不要
        if (dynamic_cast<Server *>(it->second)) {
          int server_fd = it->first;

          if (FD_ISSET(server_fd, &rfd_set)) {
            Client *client = new Client();
            client->socket_status = READ_CLIENT;
            int client_fd = client->makeSocket(server_fd);

            FD_SET(client_fd, &master_set);
            if (client_fd > max_fd) max_fd = client_fd;
            sockets[client_fd] = client;

            break;
          }
        }

        // クライアントソケットでread/writeの可能性は全部で6パターン
        if (dynamic_cast<Client *>(it->second)) {
          int client_fd = it->first;
          Client *client = dynamic_cast<Client *>(sockets[client_fd]);

          // パターン１：クライアントからrecvする
          // READ_CLIENT（最初はみんなこれ
          if (client->socket_status == READ_CLIENT &&
              FD_ISSET(client_fd, &rfd_set)) {
            long ret = client->recv(client_fd);

            if (ret == -1) {
              close(client_fd);
              FD_CLR(client_fd, &rfd_set);
              FD_CLR(client_fd, &master_set);
              delete it->second;
              sockets.erase(it);
              throw std::runtime_error("recv error\n");
            } else if (ret == 0) {
              close(client_fd);
              FD_CLR(client_fd, &rfd_set);
              FD_CLR(client_fd, &master_set);
              delete it->second;
              sockets.erase(it);
            } else {
              // パースしてつぎのsocket_statusをうまい具合に設定したい！
              // client->ParseRequest();　//
              // 全部recvできてなてなかったら次もREAD_CLIENT

              // read/writeの前までつくる
              client->makeResponse();

              // この処理はレスポンスが完成したらするから、パターン2~5の中でやると思う
              client->socket_status = WRITE_CLIENT;
            }
            break;
          }

          // パターン２：特定のファイルをreadする
          // READ_FILE
          if (client->socket_status == READ_FILE &&
              FD_ISSET(client_fd, &rfd_set)) {
            // 単なるGETとか
            break;
          }

          // パターン３：特定のファイルにwriteする
          // WRITE_FILE
          if (client->socket_status == WRITE_FILE &&
              FD_ISSET(client_fd, &wfd_set)) {
            // POSTでファイルuploadとか
            break;
          }

          // パターン４：CGIにこっちの標準入力をwriteする
          // WRITE_CGI
          if (client->socket_status == WRITE_CGI &&
              FD_ISSET(client_fd, &wfd_set)) {
            client->socket_status = WRITE_CLIENT;
            break;
          }

          // パターン５：CGIの標準出力をreadする（パターン４の次に来るところ）
          // READ_CGI
          if (client->socket_status == READ_CGI &&
              FD_ISSET(client_fd, &rfd_set)) {
            //

            break;
          }

          // パターン６：クライアントにsendする
          // WRITE_CLIENT（最後もみんなこれ）
          if (client->socket_status == WRITE_CLIENT &&
              FD_ISSET(client_fd, &wfd_set)) {
            // 完成したレスポンスを送る
            long ret = client->send(client_fd);

            if (ret == -1) {
              close(client_fd);
              FD_CLR(client_fd, &rfd_set);
              FD_CLR(client_fd, &master_set);
              sockets.erase(it);
              // writable_client_fds.erase(it);
              throw std::runtime_error("send error\n");
            } else {
              // TODO: can we send all data by one send(2)?
              // writable_client_fds.erase(it);
              // 次のrequestを待つ
              client->socket_status = READ_CLIENT;
            }
            break;
          }
        }
      }

    } else {
      throw std::runtime_error("select error\n");
    }
  }
}

/*

selectぐるぐる

・アクセプトするソケット
|
|
・クライアントからrecvするソケット
|
|       (あいまい)
cgi? --n--GET,DELETE的-y-・ファイルをreadするソケット--|
|          |                                      |
y          n----・ファイルにwriteするソケット------- |
|                                              |
|
|
(forkしてinfdとoutfdをゲット)                   |
|                                            |
|                                            |
----cgiでinfdにwriteするソケット                |
          |                                  |
          |___・cgiでoutfdからreadするソケット-|
                                            |
                                           |
                                           |
                                          ・クライアントにsendするソケット

*/