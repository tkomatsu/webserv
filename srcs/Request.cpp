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

Request::Request() : status_(STARTLINE) {
  ParseMessage();
}

Request::~Request() {}

void Request::AppendRawData(std::string raw) {
  HttpMessage::AppendRawData(raw);
  ParseMessage();
}

enum Method Request::GetMethod() const {
  if (method_ == UNKNOWN || INVALID <= method_) {
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

enum Request::ParseStatus Request::GetStatus() const {
  return status_;
}

void Request::ParseMessage() {
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

void Request::ParseStartline() {
  if (raw_.find("\r\n") == std::string::npos)
    throw ParseStartlineException("Incomplete startline");
  if (status_ == STARTLINE) {
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
    status_ = HEADER;
  }
}

void Request::ParseHeader() {
  if (status_ == HEADER) {
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
    status_ = BODY;
  }
}

void Request::ParseBody() {
  if (status_ == BODY) {
    bool flag = false;
    try { /* not chunked */
      std::string::size_type len = strtoul(GetHeader("Content-Length").c_str(), NULL, 10);
      if (raw_.size() == len) {
        body_ += raw_;
        raw_ = "";
        status_ = DONE;
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
          status_ = DONE;
      }
    } catch (HeaderKeyException) {
      if (raw_.empty()) {
        status_ = DONE;
        return ;
      }
      if (flag)
        throw RequestFatalException("Length Required");
    }
  }
}
