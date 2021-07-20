#include "Request.hpp"

Request::Request() : status_(PARSE_INIT) {
  ParseRequest();
}

void Request::AppendRawData(std::string raw) {
  raw_ += raw;
  ParseRequest();
}

void Request::ParseRequest() {
  try {
    size_t idx = 0;
    idx = ParseStartline(idx);
    idx = ParseHeader(idx);
    idx = ParseBody(idx);
  } catch (ParseStartlineException& e) {
    /* Incomplete parsing startline */
  } catch (ParseHeaderException& e) {
    /* Incomplete parsing header */
  } catch (ParseBodyException& e) {
    /* Parsing header completed. Imcomplete parsing body */
  }

  method_ = GET;
  uri_ = "/";
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

size_t Request::ParseStartline(size_t idx) {
  if (raw_.find("\r\n") == std::string::npos)
    throw ParseStartlineException("not yet");
  return idx;
}

size_t Request::ParseHeader(size_t idx) {
  return idx;
}

size_t Request::ParseBody(size_t idx) {
  return idx;
}