#include "Client.hpp"

#include <unistd.h>

#include "utility.hpp"

const int Client::buf_max = 8192;

int Client::SetSocket(int _fd) {
  socklen_t len = sizeof(addr);
  int fd = accept(_fd, (struct sockaddr *)&addr, &len);

  port = ntohs(addr.sin_port);
  hostIp = ft_inet_ntoa(addr.sin_addr);

  if (fd == -1) throw std::runtime_error("accept error\n");
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");
  return fd;
}

// TODO: make good response content
void Client::MakeResponse(void) {
  // std::ifstream ifs("./docs/html/index.html");
  // std::string header;
  // std::string body;
  // std::string line;

  /* if (ifs.fail()) throw std::runtime_error("file open error\n");
  while (getline(ifs, line)) body += line + "\n";
  ifs.close(); */

  // parse

  //　本当はパース結果によってただファイルを読んだり、ファイルに書き込んだり
  // いろんなパターンが考えられるけど
  // 今はリクエストきたらとりま決め打ちでCGI
  GenProcessForCGI();
  response.SetVersion("1.1");
  response.SetStatusCode(200);
  response.SetReason("OK");

  response.AppendHeader("Content-Type", "text/html");
  response.AppendHeader("Server", "Webserv");
  response.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");
  SetStatus(WRITE_CGI);
}

int Client::recv(int client_fd) {
  int ret;
  char buf[Client::buf_max] = {0};

  ret = ::recv(client_fd, buf, Client::buf_max - 1, 0);

  if (ret >= 0) {
    size_t i = std::string(buf).find("\r\n\r\n");
    if (i != std::string::npos) {
      std::cout << "\nrecv from " + hostIp << ":" << port << std::endl;

    } else {
      // TODO: if size of data exceed WebServ::buf_max,
      // we need to stock them.
    }
  }

  return ret;
}

int Client::send(int client_fd) {
  int ret;

  ret = ::send(client_fd, response.Str().c_str(), response.Str().size(), 0);
  if (ret >= 0) {
    std::cout << "send to   " + hostIp << ":" << port << std::endl;
  }

  return ret;
}

void Client::GenProcessForCGI(void) {
  char **args = ft_split("./docs/perl.cgi+mcgee+mine", '+');
  char envs_zikauchi[20][50] = {
      "AUTH_TYPE=",
      "CONTENT_LENGTH=",
      "CONTENT_TYPE=",
      "GATEWAY_INTERFACE=CGI/1.1",
      "PATH_INFO=",
      "PATH_TRANSLATED=",
      "QUERY_STRING=aaa+bbb",
      "REMOTE_ADDR=172.17.0.1",
      "REMOTE_IDENT=",
      "REMOTE_USER=",
      "REQUEST_METHOD=GET",
      "REQUEST_URI=",
      "SCRIPT_NAME=/docs/perl.cgi",
      "SERVER_NAME=",
      "SERVER_PORT=4200",
      "SERVER_PROTOCOL=HTTP/1.1",
      "SERVER_SOFTWARE=webserv",
  };
  char **envs = (char **)malloc(sizeof(char *) * 18);
  int i = 0;
  while (i < 17) {
    envs[i] = strdup(envs_zikauchi[i]);
    ++i;
  }
  envs[i] = NULL;

  // [0]が読み取り [1]が書き込み
  int pipe_write[2];
  int pipe_read[2];
  pid_t pid;
  pipe(pipe_write);
  pipe(pipe_read);

  if ((pid = fork()) == 0) {
    dup2(pipe_write[0], STDIN_FILENO);
    dup2(pipe_read[1], STDOUT_FILENO);

    close(pipe_write[0]);
    close(pipe_write[1]);
    close(pipe_read[0]);
    close(pipe_read[1]);

    execve("./docs/perl.cgi", args, envs);
    exit(EXIT_FAILURE);
  }

  write_cgi_fd = pipe_write[1];
  fcntl(write_cgi_fd, F_SETFL, O_NONBLOCK);
  close(pipe_write[0]);

  read_cgi_fd = pipe_read[0];
  fcntl(read_cgi_fd, F_SETFL, O_NONBLOCK);
  close(pipe_read[1]);

  i = 0;
  while (args[i]) {
    free(args[i++]);
  }
  free(args);

  i = 0;
  while (envs[i]) {
    free(envs[i++]);
  }
  free(envs);
}
