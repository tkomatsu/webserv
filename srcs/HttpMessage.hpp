#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H

#include <map>
#include <string>
#include <stdexcept>
#include <ctime>
#include <sstream>

#include "utility.hpp"

class HttpMessage {

 public:
  enum ParseStatus {
    STARTLINE,
    HEADER,
    BODY,
    DONE,
  };

 protected:
  typedef std::map<std::string, std::string> http_header;

  enum ParseStatus status_;

  std::string raw_;

  std::string http_version_;
  http_header headers_;
  std::string body_;

  HttpMessage(const HttpMessage&);
  HttpMessage& operator=(const HttpMessage&);

  virtual void AppendRawData(std::string raw);
  virtual void ParseMessage();
  virtual void ParseStartline();
  virtual void ParseHeader();
  virtual void ParseBody();

 public:
  HttpMessage();
  ~HttpMessage();

  void SetVersion(std::string version);
  void AppendHeader(std::string key, std::string value);
  void AppendHeader(std::pair<std::string, std::string> pair);
  void AppendBody(std::string);

  const std::string& GetVersion() const;
  const http_header& GetAllHeader() const;
  const std::string& GetHeader(const std::string& key) const;
  const std::string& GetBody() const;

  std::string Now(time_t time = std::time(NULL)) const;

  // Parse exception
  class ParseBodyException : public std::runtime_error {
   public:
    ParseBodyException(const std::string& what) : std::runtime_error(what) {}
  };
  class ParseHeaderException : public std::runtime_error {
   public:
    ParseHeaderException(const std::string& what) : std::runtime_error(what) {}
  };
  class ParseStartlineException : public std::runtime_error {
   public:
    ParseStartlineException(const std::string& what) : std::runtime_error(what) {}
  };

  class HeaderKeyException : public std::invalid_argument {
   public:
    HeaderKeyException(const std::string& what) : std::invalid_argument(what) {}
  };

};

#endif /* HTTPMESSAGE_H */
