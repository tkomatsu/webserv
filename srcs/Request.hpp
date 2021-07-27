#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <string>
#include <cstdlib>

#include "HttpMessage.hpp"

enum ParseStatus {
  INIT, STARTLINE, HEADER, BODY, DONE,
//  PARSE_INIT, PARSE_STARTLINE, PARSE_HEADER, PARSE_BODY, PARSE_COMPLETE,
};

class Request : public HttpMessage {

 private:
  Request(const Request& other);
  Request& operator=(const Request& rhs);

  std::string raw_;
  enum ParseStatus status_;

  enum Method method_;
  std::string uri_;

 public:
  Request();
  ~Request();

  void AppendRawData(std::string raw);

  enum Method GetMethod() const;
  const std::string& GetURI() const;

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

  // Fatal exception
  class RequestFatalException : public std::runtime_error {
   public:
    RequestFatalException(const std::string& what) : std::runtime_error(what) {}
  };

 private:
  void ParseRequest();
  void ParseStartline();
  void ParseHeader();
  void ParseBody();

};

#endif /* REQUEST_HPP */
