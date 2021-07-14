#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>

#include "utility.hpp"

class Request {
 private:
  // enum Method method_;
  std::string request_target_;
  std::string http_version_;
  std::map<std::string, std::string> headers_;
  std::vector<std::string> body_;
};

#endif /* REQUEST_HPP */