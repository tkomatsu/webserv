#include "HttpMessage.hpp"

#include "utility.hpp"

HttpMessage::HttpMessage() : status_(STARTLINE) {}

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

void HttpMessage::AppendRawData(std::string str) {
  raw_ += str;
  ParseMessage();
}

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

void HttpMessage::ParseMessage() {
  try {
    ParseStartline();
    ParseHeader();
    ParseBody();
  } catch (ParseStartlineException& e) {
    status_ = STARTLINE;
  } catch (ParseHeaderException& e) {
    status_ = HEADER;
  } catch (ParseBodyException& e) {
    status_ = BODY;
  }
}

void HttpMessage::ParseHeader() {
  if (status_ == HEADER) {
    if (raw_.find("\r\n\r\n") == std::string::npos)
      throw ParseHeaderException("Incomplete header");
    std::string flat = raw_.substr(0, raw_.find("\r\n\r\n"));
    std::vector<std::string> all = ft::vsplit(flat, '\r');
    for (int i = 0; i < all.size(); i++) {
      all[i] = ft::trim(all[i], "\n");
    }
    for (int i = 0; i < all.size(); i++) {
      std::pair<std::string, std::string> header = ft::div(all[i], ':');
      AppendHeader(header);
    }
    raw_ = raw_.substr(raw_.find("\r\n\r\n") + 4);
    status_ = BODY;
  }
}

void HttpMessage::ParseStartline() {
  if (status_ == STARTLINE) {
    status_ = HEADER;
  }
}

void HttpMessage::ParseBody() {
  if (status_ == BODY) {
    status_ = DONE;
  }
}
