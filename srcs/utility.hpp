#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <netinet/in.h>

#include <cstdlib>
#include <ctime>
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
  WRITE_CGI,
  WRITE_FILE,
  WRITE_CLIENT,
};

namespace ft {

char** split(std::string s, char c);
std::string inet_ntoa(struct in_addr in);
std::string ltoa(long num);
std::string trim(std::string s, std::string delim = " \t\v");
std::vector<std::string> vsplit(std::string s, char delim);
std::pair<std::string, std::string> div(std::string s, char delim);
std::string AutoIndexNow(time_t time);
int strcasecmp(const std::string &a, const std::string &b);

}  // namespace ft

#endif /* UTILITY_HPP */
