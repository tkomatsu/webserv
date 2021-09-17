#include "Request.hpp"

Request::Request() : method_(INVALID) {}

Request::~Request() {}

enum Method Request::GetMethod() const { return method_; }

const std::string Request::GetURI() const {
  if (uri_.empty()) {
    throw RequestFatalException("URI is not defined");
  }
  std::vector<std::string> uri_vector = ft::vsplit(uri_, '?');
  return uri_vector[0];
}

const std::string Request::GetQueryString() const {
  if (uri_.empty()) {
    throw RequestFatalException("URI is not defined");
  }
  std::vector<std::string> uri_vector = ft::vsplit(uri_, '?');
  if (uri_vector.size() >= 2)
    return uri_vector[1];
  else
    return "";
}

enum Request::ParseStatus Request::GetStatus() const { return status_; }

void Request::EraseBody(ssize_t length) {
  body_.erase(body_.begin(), body_.begin() + length);
}

void Request::Clear() {
  HttpMessage::Clear();
  method_ = INVALID;
  uri_.clear();
}

void Request::ParseStartline() {
  if (status_ != STARTLINE) return;

  std::vector<unsigned char>::iterator it;
  if ((it = std::search(raw_.begin(), raw_.end(), delim_.begin(),
                        delim_.end())) == raw_.end())
    throw ParseHeaderException("No delimiter");

  std::string startline;
  std::copy(raw_.begin(), it, std::back_inserter(startline));

  std::vector<std::string> startline_splitted = ft::vsplit(startline, ' ');
  if (startline_splitted.size() != 3)
    throw RequestFatalException("Invalid startline");
  std::string method = startline_splitted[0];
  uri_ = startline_splitted[1];
  if (method == "GET") {
    method_ = GET;
  } else if (method == "POST") {
    method_ = POST;
  } else if (method == "DELETE") {
    method_ = DELETE;
  } else {
    throw RequestFatalException("Invalid method");
  }
  if (startline_splitted[2].find("HTTP/") == std::string::npos)
    throw RequestFatalException("Invalid HTTP version");
  http_version_ = startline_splitted[2].substr(5);
  raw_ = std::vector<unsigned char>(it + delim_.size(), raw_.end());
  status_ = HEADER;
}

void Request::ParseHeader() { HttpMessage::ParseHeader(); }

void Request::ParseMessage() { HttpMessage::ParseMessage(); }

void Request::ParseBody() {
  if (status_ == BODY) {
    bool flag = false;
    try { /* not chunked */
      std::string::size_type len =
          strtoul(GetHeader("Content-Length").c_str(), NULL, 10);
      if (raw_.size() == len) {
        body_.insert(body_.end(), raw_.begin(), raw_.end());
        raw_.clear();
        status_ = DONE;
      }
      return;
    } catch (HeaderKeyException) {
      flag = true;
    }
    try { /* chunked content */
      std::string encode = GetHeader("Transfer-Encoding");
      if (encode != "chunked")
        throw RequestFatalException("Invalid Transfer-Encoding");
      std::vector<unsigned char>::iterator it;
      while ((it = std::search(raw_.begin(), raw_.end(), delim_.begin(),
                               delim_.end())) != raw_.end()) {
        std::string chunk_size;
        std::copy(raw_.begin(), it, std::back_inserter(chunk_size));
        std::string::size_type len = strtoul(chunk_size.c_str(), NULL, 16);

        std::vector<unsigned char> data(it + delim_.size(), raw_.end());
        if (data.size() >= len) {
          std::vector<unsigned char> tmp;  // = data.substr(0, len);
          std::copy(data.begin(), data.begin() + len, std::back_inserter(tmp));
          body_.insert(body_.end(), tmp.begin(), tmp.end());
          raw_ = std::vector<unsigned char>(it + delim_.size(), raw_.end());
        }
        if (len == 0) status_ = DONE;
      }
    } catch (HeaderKeyException) {
      if (raw_.empty()) {
        status_ = DONE;
        return;
      }
      if (flag) throw RequestFatalException("Length Required");
    }
  }
}
