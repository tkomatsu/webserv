#include "Client.hpp"

#include <dirent.h>
#include <unistd.h>

#include "utility.hpp"

const int Client::buf_max = 8192;

int Client::SetSocket(int _fd) {
  socklen_t len = sizeof(addr);
  int fd = accept(_fd, (struct sockaddr *)&addr, &len);

  port = ntohs(addr.sin_port);
  host_ip_ = ft_inet_ntoa(addr.sin_addr);

  if (fd == -1) throw std::runtime_error("accept error\n");
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");
  return fd;
}

// TODO: make good response content
void Client::Prepare(void) {
  // std::ifstream ifs("./docs/html/index.html");
  // std::string header;
  // std::string body;
  // std::string line;

  /* if (ifs.fail()) throw std::runtime_error("file open error\n");
  while (getline(ifs, line)) body += line + "\n";
  ifs.close(); */

  // parse

  // int ret = request.NextStatus();
  int ret;
  // ret = READ_FILE;
  // ret = READ_FILE;
  // ret = WRITE_FILE;
  // ret = WRITE_CGI;
  ret = WRITE_CLIENT;
  SetStatus(ret);

  bool is_autoindex = true;

  if (ret == READ_FILE) {
    read_fd_ = open("./docs/html/index.html", O_RDONLY);
    fcntl(read_fd_, F_SETFL, O_NONBLOCK);

    response.SetStatusCode(200);
    response.AppendHeader("Content-Type", "text/html");
    response.AppendHeader("Server", "Webserv");
    response.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");
  } else if (ret == WRITE_FILE) {
    write_fd_ = open("./docs/html/post.html", O_RDWR | O_CREAT, 0644);
    fcntl(write_fd_, F_SETFL, O_NONBLOCK);

    response.SetStatusCode(201);
    response.AppendHeader("Content-Type", "text/html");
    response.AppendHeader("Server", "Webserv");
    response.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");
    response.AppendHeader("Content-Location", "/post.html");
  } else if (ret == WRITE_CGI) {
    response.SetStatusCode(200);
    response.AppendHeader("Content-Type", "text/html");
    response.AppendHeader("Server", "Webserv");
    response.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");
    // Content-Lengthとか今わからないやつもある
  } else if (ret == WRITE_CLIENT) {
    if (is_autoindex) {
      response.SetStatusCode(200);
      response.AppendHeader("Content-Type", "text/html");
      response.AppendHeader("Server", "Webserv");
      response.AppendHeader("Date", "Wed, 30 Jun 2021 08:25:23 GMT");

      DIR *dirp = opendir("./docs");
      struct dirent *dp;
      fileinfo info;

      index.clear();
      while ((dp = readdir(dirp)) != NULL) {
        info.dirent = dp;
        stat(("./docs/" + std::string(dp->d_name)).c_str(), &info.stat);
        index.push_back(info);
      }
      closedir(dirp);

      std::string tmp;

      tmp += "<html>\n<head><title>Index of ";
      tmp += "/";
      tmp += "</title></head>\n<body bgcolor=\"white\">\n<h1>Index of ";
      tmp += "/</h1><hr><pre><a href=\"../\">../</a>\n";

      for (std::vector<fileinfo>::iterator it = index.begin();
           it != index.end(); ++it) {
        fileinfo info = *it;

        if (info.dirent->d_name[0] == '.') continue;

        tmp += "<a href=\"" + std::string(info.dirent->d_name) + "/\">";
        if (std::string(info.dirent->d_name).length() >= 50)
          tmp += std::string(info.dirent->d_name).substr(0, 47) + "..&gt;";
        else {
          tmp += std::string(info.dirent->d_name);
          if (S_ISDIR(info.stat.st_mode)) tmp += "/";
        }
        tmp += "</a>" +
               std::string(
                   50 - std::min((size_t)50,
                                 std::string(info.dirent->d_name).length()),
                   ' ');
        if (!S_ISDIR(info.stat.st_mode)) tmp += " ";
        tmp += response.Now(info.stat.st_mtimespec.tv_sec);

        tmp += std::string(19, ' ');
        if (S_ISREG(info.stat.st_mode))
          tmp += ft_ltoa(info.stat.st_size) + "\n";
        else
          tmp += "-\n";
      }
      tmp += "</pre><hr></body>\n</html>\n";

      response.SetBody(tmp);
    }
  }
}

int Client::recv(int client_fd) {
  int ret;
  char buf[Client::buf_max] = {0};

  ret = ::recv(client_fd, buf, Client::buf_max - 1, 0);

  if (ret >= 0) {
    size_t i = std::string(buf).find("\r\n\r\n");
    if (i != std::string::npos) {
      std::cout << "\nrecv from " + host_ip_ << ":" << port << std::endl;

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
    std::cout << "send to   " + host_ip_ << ":" << port << std::endl;
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
      "PATH_INFO=/perl.cgi",
      "PATH_TRANSLATED=./docs/mcgee/mine",
      "QUERY_STRING=",
      "REMOTE_ADDR=127.0.0.1",
      "REMOTE_IDENT=",
      "REMOTE_USER=",
      "REQUEST_METHOD=GET",
      "REQUEST_URI=/sample.cgi/hoge/fuga",
      "SCRIPT_NAME=/perl.cgi",
      "SERVER_NAME=localhost",
      "SERVER_PORT=4200 ",
      "SERVER_PROTOCOL=HTTP/1.1",
      "SERVER_SOFTWARE=WEBSERV/0.3",
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

  write_fd_ = pipe_write[1];
  fcntl(write_fd_, F_SETFL, O_NONBLOCK);
  close(pipe_write[0]);

  read_fd_ = pipe_read[0];
  fcntl(read_fd_, F_SETFL, O_NONBLOCK);
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
