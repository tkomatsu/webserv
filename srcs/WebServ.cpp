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
      for (std::vector<long>::iterator it_ = writable_client_fds.begin();
           it_ != writable_client_fds.end(); ++it_)
        FD_SET(*it_, &wfd_set);

      n = select(max_fd + 1, &rfd_set, &wfd_set, NULL, &timeout);
    }

    if (n > 0) {
      // accept new client
      for (std::map<long, ISocket *>::iterator it = sockets.begin();
           it != sockets.end(); ++it) {
        // if that elem is Server
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

      // recv readables
      if (n > 0)
        for (std::map<long, ISocket *>::iterator it = sockets.begin();
             it != sockets.end(); ++it) {
          if (dynamic_cast<Client *>(it->second)) {
            long client_fd = it->first;

            if (FD_ISSET(client_fd, &rfd_set)) {
              Client *client = dynamic_cast<Client *>(sockets[client_fd]);

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
                writable_client_fds.push_back(client_fd);
              }

              n = 0;
              break;
            }
          }
        }

      // send to writables
      if (n > 0)
        for (std::vector<long>::iterator it = writable_client_fds.begin();
             it != writable_client_fds.end(); ++it) {
          long client_fd = *it;

          if (FD_ISSET(client_fd, &wfd_set)) {
            Client *client = dynamic_cast<Client *>(sockets[client_fd]);

            client->makeResponse();

            long ret = client->send(client_fd);

            if (ret == -1) {
              close(client_fd);
              FD_CLR(client_fd, &rfd_set);
              FD_CLR(client_fd, &master_set);
              sockets.erase(*it);
              writable_client_fds.erase(it);
              throw std::runtime_error("send error\n");
            } else {
              // TODO: can we send all data by one send(2)?
              writable_client_fds.erase(it);
            }

            n = 0;
            break;
          }
        }

    } else {
      throw std::runtime_error("select error\n");
    }
  }
}
