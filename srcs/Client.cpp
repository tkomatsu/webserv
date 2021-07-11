#include "Client.hpp"

const int Client::buf_max = 8192;

std::string ft_inet_ntoa(struct in_addr in) {
  char buffer[18];
  std::string ret;

  unsigned char *bytes = (unsigned char *)&in;
  sprintf(buffer, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
  ret = buffer;
  return ret;
}

long Client::makeSocket(long _fd) {
  socklen_t len = sizeof(addr);
  long fd = accept(_fd, (struct sockaddr *)&addr, &len);

  port = ntohs(addr.sin_port);
  hostIp = ft_inet_ntoa(addr.sin_addr);

  if (fd == -1) throw std::runtime_error("accept error\n");
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");
  return fd;
}

// TODO: make good response content
void Client::makeResponse(void) {
  std::ifstream ifs("./docs/html/index.html");
  std::string header;
  std::string body;
  std::string line;

  if (ifs.fail()) throw std::runtime_error("file open error\n");
  while (getline(ifs, line)) body += line + "\n";
  ifs.close();

  header =
      "HTTP/1.1 200 OK\r\n"
      "Content-Length: " +
      std::to_string(body.length()) +
      "\r\n"
      "Content-Type: text/html; charset=UTF-8\r\n"
      "Date: Wed, 30 Jun 2021 08:25:23 GMT\r\n"
      "Server: Webserv\r\n"
      "\r\n";

  response = header + body;
}

long Client::recv(long client_fd) {
  long ret;
  char buf[Client::buf_max] = {0};

  ret = ::recv(client_fd, buf, Client::buf_max - 1, 0);

  if (ret >= 0) {
    size_t i = std::string(buf).find("\r\n\r\n");
    if (i != std::string::npos) {
      std::cout << "recv from " + hostIp << ":" << port << std::endl;
      /* std::cout << "\n\x1b[35m████ Request "
                   "████\n";
      std::cout << buf;
      std::cout << "████ Request "
                   "████\x1b[39m\n"; */
    } else {
      // TODO: if size of data exceed WebServ::buf_max,
      // we need to stock them.
    }
  }

  return ret;
}

long Client::send(long client_fd) {
  long ret;

  ret = ::send(client_fd, response.c_str(), response.size(), 0);

  if (ret >= 0) {
    std::cout << "send to   " + hostIp << ":" << port << std::endl;

    /* std::cout << "\n\x1b[36m████ Response ████\n";
    std::cout << response;
    std::cout << "████ Response ████\x1b[39m\n"; */
  }

  return ret;
}
