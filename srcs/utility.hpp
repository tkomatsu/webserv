#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <stdexcept>

enum Method {
  GET, POST, DELETE, INVALID,
};

// exception
class StatusException : public std::domain_error {
 public:
  StatusException(const std::string& what);
};

class HeaderKeyException : public std::invalid_argument {
 public:
  HeaderKeyException(const std::string& what);
};

class ParseBodyException : public std::runtime_error {
 public:
  ParseBodyException(const std::string& what);
};

class ParseHeaderException : public std::runtime_error {
 public:
  ParseHeaderException(const std::string& what);
};

#endif /* UTILITY_HPP */
