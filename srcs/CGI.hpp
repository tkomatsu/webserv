#ifndef CGI_HPP
#define CGI_HPP

#include <string.h>

#include <map>
#include <vector>

#include "Request.hpp"
#include "config.hpp"

class CGI {
 public:
  CGI(const Request &request, int client_port, std::string client_host,
      const config::Config &config, const std::string &path_translated);
  ~CGI();

  static const int num_envs_;
  static const std::string methods_[4];

  char **GetArgs() const { return args_; };
  char **GetEnvs() const { return envs_; };

 private:
  CGI(const CGI &);
  CGI &operator=(const CGI &);

  void SetArgs();
  void SetEnvs();
  void CalcEnvs();

  char **args_;
  char **envs_;

  const Request &request_;
  int client_port_;
  std::string client_host_;
  std::string path_translated_;
  const config::Config &config_;

  std::map<std::string, std::string> envs_map_;
};

#endif /* CGI_HPP */
