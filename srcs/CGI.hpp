#ifndef CGI_HPP
#define CGI_HPP

#include <string.h>
#include <unistd.h>

#include <map>
#include <vector>

#include "Request.hpp"
#include "config.hpp"

class CGI {
 public:
  typedef std::map<std::string, std::string> envs_map;

  CGI(const Request &request, int client_port, std::string client_host,
      const config::Config &config, const std::string &path_translated,
      const std::string &path_info);
  ~CGI();

  static const std::string methods_[4];

  void Exec();

 private:
  enum Language { PHP, PYTHON, INVALID };

  /* prohibit copy constructor and assignment operator */
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
  std::string path_info_;
  const config::Config &config_;

  enum Language lang;

  envs_map envs_map_;
};

#endif /* CGI_HPP */
