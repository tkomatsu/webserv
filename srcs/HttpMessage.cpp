#include "HttpMessage.hpp"

HttpMessage::HttpMessage() : status_(STARTLINE), delim_("\r\n") {}

HttpMessage::~HttpMessage() {}

void HttpMessage::SetVersion(const std::string& version) {
  http_version_ = version;
}

void HttpMessage::AppendHeader(const std::string& key,
                               const std::string& value) {
  std::string k = ft::trim(key);
  std::string v = ft::trim(value);
  if (headers_.find(k) == headers_.end()) {
    headers_[k] = v;
  } else {
    headers_[k] = headers_[k] + (headers_[k].empty() ? "" : ",") + v;
  }
}

void HttpMessage::AppendHeader(
    const std::pair<std::string, std::string>& pair) {
  AppendHeader(pair.first, pair.second);
}

void HttpMessage::AppendBody(const std::string& str) {
  std::vector<unsigned char> v(str.begin(), str.end());
  body_.insert(body_.end(), v.begin(), v.end());
}

void HttpMessage::AppendRawData(const std::string& str) {
  raw_ += str;
  ParseMessage();
}

void HttpMessage::RemoveHeader(const std::string& key) {
  http_header::iterator it = headers_.find(key);
  if (it != headers_.end()) {
    headers_.erase(it);
  }
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

const std::vector<unsigned char>& HttpMessage::GetBody() const { return body_; }

std::string HttpMessage::Now(time_t time) const {
  char buf[80];
  std::strftime(buf, sizeof(buf), "%a, %b %Y %H:%M:%S GMT", std::gmtime(&time));
  return std::string(buf);
}

void HttpMessage::Clear() {
  status_ = STARTLINE;
  http_version_ = "1.1";
  headers_.clear();
  body_.clear();
  raw_.clear();
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
    if (raw_.find(delim_ + delim_) == std::string::npos)
      throw ParseHeaderException("Incomplete header");
    std::string flat = raw_.substr(0, raw_.find(delim_ + delim_));
    std::vector<std::string> all = ft::vsplit(flat, '\n');
    if (delim_ == "\r\n") {
      for (std::string::size_type i = 0; i < all.size(); i++) {
        all[i] = ft::trim(all[i], "\r");
      }
    }
    for (std::string::size_type i = 0; i < all.size(); i++) {
      std::pair<std::string, std::string> header = ft::div(all[i], ':');
      AppendHeader(header);
    }
    raw_ = raw_.substr(raw_.find(delim_ + delim_) + delim_.size() * 2);
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
