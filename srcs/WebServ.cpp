#include "WebServ.hpp"

const std::string WebServ::default_path = "./conf/default.conf";
const int WebServ::buf_max = 8192;

WebServ::WebServ(const std::string &path) {
  max_fd = 0;
  // FD_ZERO(&master_set);

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
  while (true) {
    int n = 0;
    fd_set rfd_set, wfd_set;
    struct timeval timeout = (struct timeval){1, 0};

    // selecting readables or writables
    while (n == 0) {
      FD_ZERO(&rfd_set);
      FD_ZERO(&wfd_set);

      for (std::map<long, ISocket *>::iterator it = sockets.begin();
           it != sockets.end(); ++it) {
        // set listening sockets
        if (dynamic_cast<Server *>(it->second)) {
          int serevr_fd = it->first;

          FD_SET(serevr_fd, &rfd_set);
          max_fd = std::max(max_fd, serevr_fd);
        } else if (dynamic_cast<Client *>(it->second)) {
          int client_fd = it->first;
          Client *client = dynamic_cast<Client *>(sockets[client_fd]);

          if (client->socket_status == READ_CLIENT) {
            FD_SET(client_fd, &rfd_set);
            max_fd = std::max(max_fd, client_fd);
          } else if (client->socket_status == WRITE_CLIENT) {
            FD_SET(client_fd, &wfd_set);
            max_fd = std::max(max_fd, client_fd);
          } else if (client->socket_status == READ_CGI) {
            FD_SET(client->read_cgi_fd, &rfd_set);
            max_fd = std::max(max_fd, client->read_cgi_fd);
          } else if (client->socket_status == WRITE_CGI) {
            FD_SET(client->write_cgi_fd, &wfd_set);
            max_fd = std::max(max_fd, client->write_cgi_fd);
          }
        }
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

            // FD_SET(client_fd, &master_set);
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
              delete it->second;
              sockets.erase(it);
              throw std::runtime_error("recv error\n");
            } else if (ret == 0) {
              close(client_fd);
              delete it->second;
              sockets.erase(it);
            } else {
              // パースしてつぎのsocket_statusをうまい具合に設定したい！
              // client->ParseRequest();　//
              // 全部recvできてなてなかったら次もREAD_CLIENT

              // read/writeの前までつくる
              client->makeResponse();

              // この処理はレスポンスが完成したらするから、パターン2~5の中でやると思う
              // client->socket_status = WRITE_CLIENT;
            }
            break;
          }

          // パターン２：特定のファイルをreadする
          // READ_FILE
          if (client->socket_status == READ_FILE &&
              FD_ISSET(client_fd, &rfd_set)) {  // client_fdではない何か
            // 単なるGETとか
            break;
          }

          // パターン３：特定のファイルにwriteする
          // WRITE_FILE
          if (client->socket_status == WRITE_FILE &&
              FD_ISSET(client_fd, &wfd_set)) {  // client_fdではない何か
            // POSTでファイルuploadとか
            break;
          }

          // パターン４：CGIにこっちの標準入力をwriteする
          // WRITE_CGI
          if (client->socket_status == WRITE_CGI &&
              FD_ISSET(client->write_cgi_fd, &wfd_set)) {
            write(client->write_cgi_fd, "", 0);
            close(client->write_cgi_fd);
            client->socket_status = READ_CGI;
            break;
          }

          // パターン５：CGIの標準出力をreadする（パターン４の次に来るところ）
          // READ_CGI
          if (client->socket_status == READ_CGI &&
              FD_ISSET(client->read_cgi_fd, &rfd_set)) {
            char buf[WebServ::buf_max] = {0};
            
            std::cout << read(client->read_cgi_fd, buf, Client::buf_max - 1) << std::endl;
            
            close(client->read_cgi_fd);

            client->GetResponse().SetBody(buf);
            client->socket_status = WRITE_CLIENT;

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
              sockets.erase(it);
              throw std::runtime_error("send error\n");
            } else {
              // TODO: can we send all data by one send(2)?
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

// なぜかsetされない   ->  ゲッターを参照で返してなかった
// うまくexecされない  ->　chmod 755　したらできた