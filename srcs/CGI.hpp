#ifndef CGI_HPP
#define CGI_HPP

#include <map>

#include "Request.hpp"

class CGI {
 public:
  CGI(const Request &request, int client_port, std::string client_host, int server_port, std::string server_host);
  ~CGI();

  static const int num_envs_;

  char **GetArgs() { return args_; };
  char **GetEnvs() { return envs_; };

 private:
  char **args_;
  char **envs_;

  std::map<std::string, std::string> envs_map_;
};

#endif
