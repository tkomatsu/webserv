#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <ctime>
#include <string>

enum Method {
  GET,
  POST,
  DELETE,
  INVALID,
};

char** ft_split(std::string s, char c);
std::string ft_inet_ntoa(struct in_addr in);
std::string ft_ltoa(long num);

#endif /* UTILITY_HPP */
