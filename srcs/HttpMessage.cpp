#include "HttpMessage.hpp"

HttpMessage::HttpMessage() {}

HttpMessage::~HttpMessage() {}

class ParseBodyException : public std::runtime_error {
 public:
  ParseBodyException(const std::string& what);
};

class ParseHeaderException : public std::runtime_error {
 public:
  ParseHeaderException(const std::string& what);
};

class ParseStartlineException : public std::runtime_error {
 public:
  ParseStartlineException(const std::string& what);
};

const HttpMessage::http_header& HttpMessage::GetAllHeader() const {
  return headers_;
}

const std::string& HttpMessage::GetHeader(const std::string& key) const {
  if (headers_.find(key) == headers_.end())
    throw HeaderKeyException("No such header");
  return headers_.find(key)->second;
}

const std::string& HttpMessage::GetBody() const { return body_; }

const std::string HttpMessage::Now() const {
}