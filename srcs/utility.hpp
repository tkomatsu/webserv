#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

enum Method {
  GET,
  POST,
  DELETE,
  INVALID,
};

enum SocketStatus {
  READ_CLIENT,
  READ_FILE,
  READ_CGI,
  READ_WRITE_CGI,
  WRITE_FILE,
  WRITE_CLIENT,
};

namespace ft {

char **split(std::string s, char c);
std::string inet_ntoa(struct in_addr in);
std::string ltoa(long num);
std::string trim(std::string s, std::string delim = " \t\v");
std::vector<std::string> vsplit(std::string s, char delim);
std::pair<std::string, std::string> div(std::string s, char delim);
std::string AutoIndexNow(time_t time);
std::string what_time(void);
char **vector_to_array(std::vector<std::string> v);

struct CaseInsensitiveCompare
    : public std::binary_function<std::string, std::string, bool> {
  bool operator()(const std::string &a, const std::string &b) const;
};

class HttpResponseException : public std::runtime_error {
 public:
  HttpResponseException(const std::string &what) : std::runtime_error(what) {}
};

class ConfigException : public std::runtime_error {
 public:
  ConfigException(const std::string &what) : std::runtime_error(what) {}
};

}  // namespace ft

#endif /* UTILITY_HPP */
