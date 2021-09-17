#include "HttpMessage.hpp"

HttpMessage::HttpMessage() : status_(STARTLINE), delim_("\r\n") {}

HttpMessage::~HttpMessage() {}

void HttpMessage::AppendRawData(const char *data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    raw_.push_back(data[i]);
  }
  ParseMessage();
}

void HttpMessage::SetVersion(const std::string &version) {
  http_version_ = version;
}

void HttpMessage::AppendHeader(const std::string &key,
                               const std::string &value) {
  std::string k = ft::trim(key);
  std::string v = ft::trim(value);
  if (headers_.find(k) != headers_.end()) {
    headers_[k] += (headers_[k].empty() ? "" : ",") + v;
  } else {
    headers_[k] = v;
  }
}

void HttpMessage::AppendHeader(
    const std::pair<std::string, std::string> &pair) {
  AppendHeader(pair.first, pair.second);
}

void HttpMessage::AppendBody(std::vector<unsigned char> &data) {
  body_.insert(body_.end(), data.begin(), data.end());
}

void HttpMessage::RemoveHeader(const std::string &key) {
  http_headers::iterator it = headers_.find(key);
  if (it != headers_.end()) {
    headers_.erase(it);
  }
}

const std::string &HttpMessage::GetVersion() const { return http_version_; }

const HttpMessage::http_headers &HttpMessage::GetAllHeader() const {
  return headers_;
}

const std::string &HttpMessage::GetHeader(const std::string &key) const {
  if (headers_.find(key) == headers_.end())
    throw HeaderKeyException("No such header");
  return headers_.find(key)->second;
}

const std::vector<unsigned char> &HttpMessage::GetBody() const { return body_; }

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
  } catch (ParseStartlineException &e) {
    status_ = STARTLINE;
  } catch (ParseHeaderException &e) {
    status_ = HEADER;
  } catch (ParseBodyException &e) {
    status_ = BODY;
  }
}

void HttpMessage::ParseStartline() {
  if (status_ == STARTLINE) {
    status_ = HEADER;
  }
}

void HttpMessage::ParseHeader() {
  if (status_ != HEADER) return;

  std::vector<unsigned char>::iterator it;
  std::string delims = delim_ + delim_;
  if ((it = std::search(raw_.begin(), raw_.end(), delims.begin(),
                        delims.end())) == raw_.end())
    throw ParseHeaderException("No delimiter");

  std::string flat;
  std::copy(raw_.begin(), it, std::back_inserter(flat));

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
  raw_ = std::vector<unsigned char>(it + delims.size(), raw_.end());
  status_ = BODY;
}

void HttpMessage::ParseBody() {
  if (status_ == BODY) {
    status_ = DONE;
  }
}
