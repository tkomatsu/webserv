#include "WebServ.hpp"

const std::string WebServ::default_path = "./conf/default.conf";
const int WebServ::buf_max = 8192;

WebServ::WebServ(const std::string &path) {
  max_fd = 0;
  FD_ZERO(&master_set);

  parse(path);
}

WebServ::~WebServ() {
  for (std::map<long, ISocket *>::iterator it = sockets.begin();
       it != sockets.end(); ++it) {
    delete it->second;
  }
  sockets.clear();
}

void WebServ::parse(const std::string &path) {
  (void)path;

  for (int i = 0; i < 3; ++i) {
    Server *server = new Server(4200 + i, "127.0.0.1");
    long fd = server->makeSocket(42);  // 42 is meanless

    sockets[fd] = server;
  }
}

void WebServ::start(void) {
  for (std::map<long, ISocket *>::iterator it = sockets.begin();
       it != sockets.end(); ++it) {
    long server_fd = it->first;

    FD_SET(it->first, &master_set);
    if (server_fd > max_fd) max_fd = server_fd;
  }
  if (max_fd == 0) throw std::runtime_error("no server error\n");

  while (true) {
    int n = 0, i = 0;
    fd_set rfd_set, wfd_set;
    struct timeval timeout = (struct timeval){1, 0};
    std::string message = "😺😺😺    ";

    while (n == 0) {
      std::cout << "\r" + message.substr(i, message.length() - i) +
                       message.substr(0, i)
                << std::flush;
      if ((i += 4) >= (int)message.length()) i = 0;

      memcpy(&rfd_set, &master_set, sizeof(master_set));
      FD_ZERO(&wfd_set);
      for (std::vector<long>::iterator it_ = writable_client_fds.begin();
           it_ != writable_client_fds.end(); ++it_)
        FD_SET(*it_, &wfd_set);

      n = select(max_fd + 1, &rfd_set, &wfd_set, NULL, &timeout);
    }

    if (n > 0) {
      for (std::vector<long>::iterator it = writable_client_fds.begin();
           it != writable_client_fds.end(); ++it) {
        long client_fd = *it;

        if (FD_ISSET(client_fd, &wfd_set)) {
          // TODO:　送る内容しっかり作ろう
          std::ifstream ifs("./docs/html/index.html");
          std::string body;
          std::string line;
          if (ifs.fail()) throw std::runtime_error("file open error\n");
          while (getline(ifs, line)) body += line + "\n";
          ifs.close();

          std::string header =
              "HTTP/1.1 200 OK\r\n"
              "Content-Length: " + std::to_string(body.length()) + "\r\n"
              "Content-Type: text/html; charset=UTF-8\r\n"
              "Date: Wed, 30 Jun 2021 08:25:23 GMT\r\n"
              "Server: Webserv\r\n"
              "\r\n";
          
          std::string response = header + body;

          long ret = send(client_fd, response.c_str(), response.size(), 0);

          if (ret == -1) {
            close(client_fd);
            FD_CLR(client_fd, &rfd_set);
            FD_CLR(client_fd, &master_set);
            sockets.erase(*it);
            writable_client_fds.erase(it);
            throw std::runtime_error("send error\n");
          } else {
            std::cout
                << "\n\x1b[36m██████████ ここから　レスポンス ██████████\n";
            std::cout << response;
            std::cout
                << "██████████ ここまで　レスポンス ██████████\x1b[39m\n\n";
            // TODO:
            // 一回のsendで送りきれないときどっかにためとく?

            writable_client_fds.erase(it);
          }

          n = 0;
          break;
        }
      }

      if (n > 0)
        for (std::map<long, ISocket *>::iterator it = sockets.begin();
             it != sockets.end(); ++it) {
          if (dynamic_cast<Client *>(it->second)) {
            long client_fd = it->first;

            if (FD_ISSET(client_fd, &rfd_set)) {
              char buf[WebServ::buf_max] = {0};
              long ret;

              ret = recv(client_fd, buf, WebServ::buf_max - 1, 0);
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
                size_t i = std::string(buf).find("\r\n\r\n");
                if (i != std::string::npos) {
                  std::cout << "\n\x1b[35m██████████ ここから　リクエスト "
                               "██████████\n";
                  std::cout << buf;
                  std::cout << "██████████ ここまで　リクエスト "
                               "██████████\x1b[39m\n\n";
                  writable_client_fds.push_back(client_fd);
                } else {
                  // TODO:

                  // 一回のrecvで取りきれないときどっかにためとく?　足りないと国庫に来る
                }
              }
              n = 0;
              break;
            }
          }
        }

      if (n > 0)
        for (std::map<long, ISocket *>::iterator it = sockets.begin();
             it != sockets.end(); ++it) {
          // そのmapの要素はServerか
          if (dynamic_cast<Server *>(it->second)) {
            long server_fd = it->first;

            if (FD_ISSET(server_fd, &rfd_set)) {
              Client *client = new Client();
              long client_fd = client->makeSocket(server_fd);

              FD_SET(client_fd, &master_set);
              if (client_fd > max_fd) max_fd = client_fd;
              sockets[client_fd] = client;

              n = 0;
              break;
            }
          }
        }

    } else {
      throw std::runtime_error("select error\n");
    }
  }
}
