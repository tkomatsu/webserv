#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H

#include <map>
#include <string>
#include <stdexcept>
#include <ctime>

#include "utility.hpp"

class HttpMessage {
 protected:
  typedef std::map<std::string, std::string> http_header;

  http_header headers_;
  std::string body_;

  HttpMessage(const HttpMessage&);
  HttpMessage& operator=(const HttpMessage&);

 public:
  HttpMessage();
  ~HttpMessage();

  void AppendHeader(std::string key, std::string value);
  void AppendHeader(std::pair<std::string, std::string> pair);
  void AppendBody(std::string);

  const http_header& GetAllHeader() const;
  const std::string& GetHeader(const std::string& key) const;
  const std::string& GetBody() const;

  class HeaderKeyException : public std::invalid_argument {
   public:
    HeaderKeyException(const std::string& what);
  };

};

#endif /* HTTPMESSAGE_H */
