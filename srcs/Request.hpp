#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "HttpMessage.hpp"

enum ParseStatus {
  INIT, STARTLINE, HEADER, BODY, DONE
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

  // Exception
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
  class ParseFatalException : public std::runtime_error {
   public:
    ParseFatalException(const std::string& what) : std::runtime_error(what) {}
  };

 private:
  void ParseRequest();
  void ParseStartline();
  void ParseHeader();
  void ParseBody();

};

#endif /* REQUEST_HPP */
