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
  struct tm* now = std::gmtime(NULL);
  std::stringstream s;
  const std::string kWkday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const std::string kMonth[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  s << kWkday[now->tm_wday] << ", ";
  s << now->tm_mday << " " << kMonth[now->tm_mon] << 1900 + now->tm_year << " ";
  s << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << " GMT";
  return s.str();
}