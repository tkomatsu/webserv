#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <ctime>
#include <stdexcept>

enum Method {
  GET,
  POST,
  DELETE,
  INVALID,
};

// exception
class StatusException : public std::domain_error {
 public:
  StatusException(const std::string& what):std::domain_error(what) {};
};

class HeaderKeyException : public std::invalid_argument {
 public:
  HeaderKeyException(const std::string& what):std::invalid_argument(what) {};
};

class ParseBodyException : public std::runtime_error {
 public:
  ParseBodyException(const std::string& what):std::runtime_error(what) {};
};

class ParseHeaderException : public std::runtime_error {
 public:
  ParseHeaderException(const std::string& what):std::runtime_error(what) {};
};

char** ft_split(std::string s, char c);
std::string ft_inet_ntoa(struct in_addr in);
std::string ft_itoa(long num);

#endif /* UTILITY_HPP */
