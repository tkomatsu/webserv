#include "Request.hpp"

namespace {

std::vector<std::string> split(std::string s, char delim) {
  std::vector<std::string> v;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    v.push_back(item);
  }
  return v;
}

std::pair<std::string, std::string> div(std::string s, char delim) {
  std::string k = s.substr(0, s.find(delim));
  std::string v = s.substr(s.find(delim) + 1);
  return std::make_pair(k, v);
}

}

Request::Request() : status_(INIT) {
  ParseRequest();
}

Request::~Request() {}

void Request::AppendRawData(std::string raw) {
  raw_ += raw;
  ParseRequest();
}

enum Method Request::GetMethod() const {
  /* この前にパースの時点で例外が吐かれるので、理論上はここに来ることはない */
  if (method_ == UNKNOWN || INVALID <= method_) {
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

void Request::ParseRequest() {
  try {
    ParseStartline();
    ParseHeader();
    ParseBody();
  } catch (ParseStartlineException& e) {
    status_ = INIT;
  } catch (ParseHeaderException& e) {
    status_ = STARTLINE;
  } catch (ParseBodyException& e) {
    status_ = HEADER;
  }
}

void Request::ParseStartline() {
  if (raw_.find("\r\n") == std::string::npos)
    throw ParseStartlineException("Incomplete startline");
  if (status_ == INIT) {
    std::string startline = raw_.substr(0, raw_.find("\r\n"));
    std::vector<std::string> startline_splitted = split(startline, ' ');
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
      method_ = INVALID;
    }
    if (method_ == INVALID)
      throw RequestFatalException("Invalid method");
    uri_ = startline_splitted[1];
    if (startline_splitted[2].find("HTTP/") == std::string::npos)
      throw RequestFatalException("Invalid HTTP version");
    http_version_ = startline_splitted[2].substr(5);
    raw_ = raw_.substr(raw_.find("\r\n") + 2);
    status_ = STARTLINE;
  }
}

void Request::ParseHeader() {
  if (status_ == STARTLINE) {
    if (raw_.find("\r\n\r\n") == std::string::npos)
      throw ParseHeaderException("Incomplete header");
    std::string flat = raw_.substr(0, raw_.find("\r\n\r\n"));
    std::vector<std::string> all = split(flat, '\r');
    for (int i = 0; i < all.size(); i++) {
      all[i] = ft::trim(all[i], "\n");
    }
    for (int i = 0; i < all.size(); i++) {
      std::pair<std::string, std::string> header = div(all[i], ':');
      AppendHeader(header);
    }
    raw_ = raw_.substr(raw_.find("\r\n\r\n") + 4);
    status_ = HEADER;
  }
}

void Request::ParseBody() {
  if (status_ == HEADER) {
    bool flag = false;
    try { /* not chunked */
      std::string::size_type len = strtoul(GetHeader("Content-Length").c_str(), NULL, 10);
      if (raw_.size() == len) {
        body_ += raw_;
        raw_ = "";
        status_ = BODY;
      }
      return ;
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
        if (len == 0)
          status_ = BODY;
      }
    } catch (HeaderKeyException) {
      if (flag)
        throw RequestFatalException("Length Required");
    }
  }
}
