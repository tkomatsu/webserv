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
    try {
      std::string::size_type len = strtoul(GetHeader("Content-Length").c_str(), NULL, 10);
      body_ += raw_;
      raw_ = "";
      if (body_.size() == len)
        status_ = BODY;
      return ;
    } catch (HeaderKeyException) {}
    try {
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

      /*
      std::string readable = raw_.substr(0, raw_.find_last_of("\r\n"));

      // 最後がチャンクのサイズ指定だった場合は、最後の数字を部分を削る
      std::string last_line = readable.substr(readable.find_last_of("\r\n") + 2);
      char *ptr;
      unsigned long len = strtoul(last_line.c_str(), &ptr, 16);
      if (ptr != NULL)
        readable = readable.substr(0, readable.find_last_of("\r\n"));

      raw_ = raw_.substr(readable.size() + 2);
      */

    } catch (HeaderKeyException) {}
  }
}
