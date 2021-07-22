#include "HttpMessage.hpp"

HttpMessage::HttpMessage() {}

HttpMessage::~HttpMessage() {}

void HttpMessage::AppendHeader(std::string key, std::string value) {
  headers_[key] = value;
}

void HttpMessage::AppendHeader(std::pair<std::string, std::string> pair) {
  headers_[pair.first] = pair.second;
}

void HttpMessage::AppendBody(std::string str) { body_ += str; }

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