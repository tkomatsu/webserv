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

std::string HttpMessage::Now(time_t time) const {
  char buf[80];
  std::strftime(buf, sizeof(buf), "%a, %b %Y %H:%M:%S GMT", std::gmtime(&time));
  return std::string(buf);
}