#include "CGI.hpp"

#include <string.h>

const int CGI::num_envs_ = 18;

CGI::CGI(const Request &request) {
  args_ = ft::split("./docs/chunked.cgi+mcgee+mine", '+');

  envs_map_["AUTH_TYPE"] = "";
  envs_map_["CONTENT_LENGTH"] = "";
  envs_map_["CONTENT_TYPE"] = "";
  envs_map_["GATEWAY_INTERFACE"] = "CGI/1.1";
  envs_map_["PATH_INFO"] = "";
  envs_map_["PATH_TRANSLATED"] = "";
  envs_map_["QUERY_STRING"] = "";
  envs_map_["REMOTE_ADDR"] = "";
  envs_map_["REMOTE_IDENT"] = "";
  envs_map_["REMOTE_USER"] = "";
  envs_map_["REQUEST_METHOD"] = "";
  envs_map_["REQUEST_URI"] = "";
  envs_map_["SCRIPT_NAME"] = "";
  envs_map_["SERVER_NAME"] = "";
  envs_map_["SERVER_PORT"] = "";
  envs_map_["SERVER_PROTOCOL"] = "HTTP/1.1";
  envs_map_["SERVER_SOFTWARE"] = "WEBSERV/0.4.2";

  // set envs according to request
  (void)request;

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

