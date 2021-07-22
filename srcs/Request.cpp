#include "Request.hpp"

Request::Request() : status_(PARSE_INIT) {
  (void)status_;
  ParseRequest();
};

Request::~Request(){};

void Request::AppendRawData(const char* raw) {
  raw_ += std::string(raw);
  ParseRequest();
}

void Request::ParseRequest() {
  try {
    ParseStartline();
    ParseHeader();
    ParseBody();
  } catch (ParseHeaderException& e) {
    /* Incomplete parsing header */
  } catch (ParseBodyException& e) {
    /* Parsing header completed. Imcomplete parsing body */
  }

  method_ = GET;
  uri_ = "/index.html";
  headers_["Content-Type"] = "text/html; charset=UTF-8";
  headers_["Date"] = "Wed, 02 Feb 2042 00:42:42 GMT";
  headers_["Server"] = "webserv";
}

enum Method Request::GetMethod() const { return method_; }

const std::string& Request::GetURI() const { return uri_; }

const std::string& Request::GetVersion() const { return http_version_; }

const std::string& Request::GetHeader(const std::string& key) const {
  if (headers_.find(key) == headers_.end())
    throw HeaderKeyException("No such header");
  return headers_.find(key)->second;
}

std::string Request::GetBody() const { return body_; }

void Request::ParseStartline() {}

void Request::ParseHeader() {}

void Request::ParseBody() {}
