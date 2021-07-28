#include "HttpMessage.hpp"
#include "utility.hpp"

HttpMessage::HttpMessage() {}

HttpMessage::~HttpMessage() {}

void HttpMessage::SetVersion(std::string version) { http_version_ = version; }

void HttpMessage::AppendHeader(std::string key, std::string value) {
  key = ft::trim(key);
  value = ft::trim(value);
  if (headers_.find(key) == headers_.end()) {
    headers_[key] = value;
  } else {
    headers_[key] = headers_[key] + (headers_[key].empty() ? "" : ",") + value;
  }
}

void HttpMessage::AppendHeader(std::pair<std::string, std::string> pair) {
  AppendHeader(pair.first, pair.second);
}

void HttpMessage::AppendBody(std::string str) { body_ += str; }

void HttpMessage::AppendRawData(std::string str) { raw_ += str; }

const std::string& HttpMessage::GetVersion() const { return http_version_; }

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