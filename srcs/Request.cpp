#include "Request.hpp"

Request::Request() : status_(INIT) {
  ParseRequest();
}

Request::~Request() {}

void Request::AppendRawData(std::string raw) {
  raw_ += raw;
  ParseRequest();
}

void Request::ParseRequest() {
  try {
    ParseStartline();
    ParseHeader();
    ParseBody();
  } catch (ParseStartlineException& e) {
    /* Incomplete parsing startline */
  } catch (ParseHeaderException& e) {
    /* Incomplete parsing header */
  } catch (ParseBodyException& e) {
    /* Parsing header completed. Imcomplete parsing body */
  } catch (ParseFatalException& e) {
    /* Parsing completed. Fatal error */
  }

  method_ = GET;
  uri_ = "/";
  headers_["Content-Type"] = "text/html; charset=UTF-8";
  headers_["Date"] = Now();
}

enum Method Request::GetMethod() const {
  if (method_ == UNKNOWN) {
    throw ParseStartlineException("Method is not defined");
  }
  return method_;
}

const std::string& Request::GetURI() const {
  if (uri_.empty()) {
    throw ParseStartlineException("URI is not defined");
  }
  return uri_;
}

void Request::ParseStartline() {
  if (raw_.find("\r\n") == std::string::npos)
    throw ParseStartlineException("Incomplete startline");
  if (status_ == INIT) {
    std::string startline = raw_.substr(0, raw_.find("\r\n"));
    std::string method = startline.substr(0, startline.find(" "));
    if (method == "GET") {
      method_ = GET;
    } else if (method == "POST") {
      method_ = POST;
    } else if (method == "DELETE") {
      method_ = DELETE;
    } else {
      method_ = INVALID;
    }
    if (method_ == INVALID)
      throw ParseFatalException("Invalid method");
    if (startline.find("HTTP/") == std::string::npos)
      throw ParseFatalException("Invalid startline");
    uri_ = startline.substr(startline.find(" ") + 1, startline.rfind(" ") - 1);
    http_version_ = startline.substr(startline.rfind(" ") + 6, startline.rfind(" ") + 8);
    if (http_version_ != "1.1")
      throw ParseFatalException("Invalid version");
    raw_ = raw_.substr(raw_.find("\r\n") + 2);
  }
}

void Request::ParseHeader() {
  if (status_ == STARTLINE) {
    if (raw_.find("\r\n\r\n") == std::string::npos)
      throw ParseHeaderException("Incomplete header");
    std::string header = raw_.substr(0, raw_.find("\r\n\r\n"));
    std::string::size_type pos = 0;
    while ((pos = header.find("\r\n", pos)) != std::string::npos) {
      std::string line = header.substr(pos, header.find("\r\n", pos) - pos);
      if (line.empty())
        break;
      std::string::size_type colon = line.find(":");
      if (colon == std::string::npos)
        throw ParseFatalException("Invalid header");
      std::string key = line.substr(0, colon - 1);
      std::string value = line.substr(colon + 1);
      value = value.substr(value.find_first_not_of(" ") + 1, value.find_last_not_of(" ") - 1);
      headers_[key] = value;
      pos += 2;
    }
    raw_ = raw_.substr(raw_.find("\r\n\r\n") + 4);
    status_ = HEADER;
  }
}

void Request::ParseBody() {
  if (status_ == HEADER) {
    body_ += raw_;
  }
}