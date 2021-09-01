#include "CGI.hpp"

#include <string.h>

#include <vector>

const int CGI::num_envs_ = 19;

const std::string CGI::methods_[4] = {"GET", "POST", "DELETE", "INVALID"};

void CGI::SetArgs() {
  std::vector<std::string> request_uri = ft::vsplit(
      request_.GetURI(), '?');  // /abc?mcgee=mine => ["/abc", "mcgee=mine"]
  // alias: ./docs/html/  config.GetAlias(request_uri[0])
  // location: /  config.GetLocation(request_uri[0])
  // request: /abc  request_uri[0]

  // std::string splited = alias + (request-path - location) + "+"
  // request.GetQueryString()　(「http://サーバー名/CGIスクリプト名?データ」というURLを要求した場合のデータ部分)
  // if ('=' not in query_string)
  //     splited += query_string
  // args_ = ft::split(splited, '+');

  args_ = ft::split("./docs/perl.cgi+mcgee+mine", '+');
}

void CGI::CalcEnvs() {
  try {
    std::string auth = request_.GetHeader("Authorization");
    envs_map_["AUTH_TYPE"] = ft::vsplit(auth, ' ')[0];
  } catch (std::exception &e) {
    envs_map_["AUTH_TYPE"] = "";
  }

  envs_map_["CONTENT_LENGTH"] = ft::ltoa(request_.GetBody().size());

  try {
    std::string content_type = request_.GetHeader("Content-type");
    envs_map_["CONTENT_TYPE"] = content_type;
  } catch (std::exception &e) {
  }

  envs_map_["GATEWAY_INTERFACE"] = "CGI/1.1";

  envs_map_["PATH_INFO"] = "";
  //    [設定]
  //      root /var/www/html
  //    [置いてあるファイル]
  //      /var/www/html/dir1/index.php

  //    http://127.0.0.1/dir1/index.php
  //    にアクセスしたら、PATH_INFOは
  //    /dir1/index.php  (== SCRIPT_NAME)
  //    PATH_TRANSLATEDは
  //    /var/www/html/dir1/index.php

  // ∴　envs_map_["PATH_INFO"] = ft::vsplit(request_.GetURI(), '?')[0];

  envs_map_["PATH_TRANSLATED"] = path_translated_;

  envs_map_["QUERY_STRING"] = request_.GetQueryString();
  envs_map_["REMOTE_ADDR"] = client_host_;
  envs_map_["REMOTE_PORT"] = ft::ltoa(client_port_);
  envs_map_["REMOTE_IDENT"] = "";
  envs_map_["REMOTE_USER"] = "";
  envs_map_["REQUEST_METHOD"] = methods_[request_.GetMethod()];
  if (request_.GetQueryString().empty())
    envs_map_["REQUEST_URI"] = request_.GetURI();
  else
    envs_map_["REQUEST_URI"] =
        request_.GetURI() + "?" + request_.GetQueryString();
  envs_map_["SCRIPT_NAME"] = envs_map_["PATH_INFO"];
  envs_map_["SERVER_NAME"] = config_.GetHost();
  envs_map_["SERVER_PORT"] = ft::ltoa(config_.GetPort());
  envs_map_["SERVER_PROTOCOL"] = "HTTP/1.1";
  envs_map_["SERVER_SOFTWARE"] = "Webserv/0.4.2";
}

void CGI::SetEnvs() {
  CalcEnvs();

  std::string tmp;
  int i = 0;
  envs_ = (char **)malloc(sizeof(char *) * CGI::num_envs_);

  tmp = "AUTH_TYPE=" + envs_map_["AUTH_TYPE"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "CONTENT_LENGTH=" + envs_map_["CONTENT_LENGTH"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "CONTENT_TYPE=" + envs_map_["CONTENT_TYPE"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "GATEWAY_INTERFACE=" + envs_map_["GATEWAY_INTERFACE"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "PATH_INFO=" + envs_map_["PATH_INFO"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "PATH_TRANSLATED=" + envs_map_["PATH_TRANSLATED"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "QUERY_STRING=" + envs_map_["QUERY_STRING"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "REMOTE_ADDR=" + envs_map_["REMOTE_ADDR"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "REMOTE_PORT=" + envs_map_["REMOTE_PORT"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "REMOTE_IDENT=" + envs_map_["REMOTE_IDENT"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "REMOTE_USER=" + envs_map_["REMOTE_USER"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "REQUEST_METHOD=" + envs_map_["REQUEST_METHOD"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "REQUEST_URI=" + envs_map_["REQUEST_URI"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "SCRIPT_NAME=" + envs_map_["SCRIPT_NAME"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "SERVER_NAME=" + envs_map_["SERVER_NAME"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "SERVER_PORT=" + envs_map_["SERVER_PORT"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "SERVER_PROTOCOL=" + envs_map_["SERVER_PROTOCOL"];
  envs_[i++] = strdup(tmp.c_str());
  tmp = "SERVER_SOFTWARE=" + envs_map_["SERVER_SOFTWARE"];
  envs_[i++] = strdup(tmp.c_str());

  envs_[i] = NULL;
}

CGI::CGI(const Request &request, int client_port, std::string client_host,
         const config::Config &config, const std::string &path_translated)
    : request_(request),
      client_port_(client_port),
      client_host_(client_host),
      path_translated_(path_translated),
      config_(config) {
  SetArgs();
  SetEnvs();
}

CGI::~CGI() {
  int i = 0;

  while (args_[i]) {
    free(args_[i++]);
  }
  free(args_);

  i = 0;
  while (envs_[i]) {
    free(envs_[i++]);
  }
  free(envs_);
}
