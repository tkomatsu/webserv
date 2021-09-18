#include "CGI.hpp"

const std::string CGI::methods_[4] = {"GET", "POST", "DELETE", "INVALID"};

CGI::CGI(const Request &request, int client_port, std::string client_host,
         const config::Config &config, const std::string &path_translated,
         const std::string &path_info)
    : request_(request),
      client_port_(client_port),
      client_host_(client_host),
      path_translated_(path_translated),
      path_info_(path_info),
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

void CGI::Exec(void) {
  std::string path(getenv("PATH"));
  std::string executable[INVALID] = {"php-cgi", "python3"};
  std::vector<std::string> path_splited = ft::vsplit(path, ':');

  for (std::vector<std::string>::const_iterator it = path_splited.begin();
       it != path_splited.end(); ++it) {
    execve((*it + "/" + executable[lang]).c_str(), args_, envs_);
  }
  exit(EXIT_FAILURE);
}

void CGI::SetArgs() {
  std::vector<std::string> args_vector;

  size_t i = path_info_.length() - 1;
  while (i > 0 && path_info_[i] != '.') --i;
  std::string extension = path_info_.substr(i);

  if (extension == ".php") {
    lang = PHP;
    args_vector.push_back("php-cgi");
  } else if (extension == ".py") {
    lang = PYTHON;
    args_vector.push_back("python3");
  } else {
    lang = INVALID;
    exit(EXIT_FAILURE);
  }

  args_vector.push_back(path_translated_);

  std::string query = request_.GetQueryString();
  if (query.find('=') == std::string::npos) {
    if (query.find('+') != std::string::npos) {
      std::vector<std::string> query_vector = ft::vsplit(query, '+');

      for (std::vector<std::string>::const_iterator itr = query_vector.begin();
           itr != query_vector.end(); ++itr) {
        args_vector.push_back(*itr);
      }
    } else {
      args_vector.push_back(query);
    }
  }

  args_ = ft::vector_to_array(args_vector);
}

void CGI::SetEnvs() {
  CalcEnvs();

  std::string tmp;
  int i = 0;
  envs_ = reinterpret_cast<char **>(
      malloc(sizeof(char *) * (envs_map_.size() + 1)));
  if (!envs_) exit(EXIT_FAILURE);
  for (envs_map::iterator it = envs_map_.begin(); it != envs_map_.end(); it++) {
    tmp = it->first + "=" + it->second;
    envs_[i++] = strdup(tmp.c_str());
  }
  envs_[i] = NULL;
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
  envs_map_["PATH_INFO"] = path_info_;
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
