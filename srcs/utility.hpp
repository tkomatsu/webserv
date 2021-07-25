#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <netinet/in.h>

#include <ctime>
#include <string>

enum Method {
  UNKNOWN,
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

}  // namespace ft

#endif /* UTILITY_HPP */
