#include "Request.hpp"

Request::Request() : method_(INVALID) { status_ = STARTLINE; }

Request::~Request() {}

void Request::AppendRawData(std::string raw) {
  HttpMessage::AppendRawData(raw);
}

enum Method Request::GetMethod() const {
  if (method_ == INVALID) {
    /* この前にパースの時点で例外が吐かれるので、理論上はここに来ることはない */
    throw RequestFatalException("Method is not defined");
  }
  return method_;
}

const std::string& Request::GetURI() const {
  if (uri_.empty()) {
    throw RequestFatalException("URI is not defined");
  }
  return uri_;
}

enum Request::ParseStatus Request::GetStatus() const { return status_; }

void Request::ParseStartline() {
  if (raw_.find("\r\n") == std::string::npos)
    throw ParseStartlineException("Incomplete startline");
  if (status_ == STARTLINE) {
    std::string startline = raw_.substr(0, raw_.find("\r\n"));
    std::vector<std::string> startline_splitted = ft::vsplit(startline, ' ');
    if (startline_splitted.size() != 3)
      throw RequestFatalException("Invalid startline");
    std::string method = startline_splitted[0];
    if (method == "GET") {
      method_ = GET;
    } else if (method == "POST") {
      method_ = POST;
    } else if (method == "DELETE") {
      method_ = DELETE;
    } else {
      throw RequestFatalException("Invalid method");
    }
    uri_ = startline_splitted[1];
    if (startline_splitted[2].find("HTTP/") == std::string::npos)
      throw RequestFatalException("Invalid HTTP version");
    http_version_ = startline_splitted[2].substr(5);
    raw_ = raw_.substr(raw_.find("\r\n") + 2);
    status_ = HEADER;
  }
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
        body_ += raw_;
        raw_ = "";
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
      while (raw_.find("\r\n") != std::string::npos) {
        std::string::size_type len = strtoul(raw_.c_str(), NULL, 16);
        std::string data = raw_.substr(raw_.find("\r\n") + 2);
        if (data.size() >= len) {
          body_ += data.substr(0, len);
          raw_ = raw_.substr(raw_.find("\r\n") + 2 + len + 2);
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
