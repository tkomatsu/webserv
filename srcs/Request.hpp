#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "HttpMessage.hpp"

enum ParseStatus {
  PARSE_INIT, PARSE_STARTLINE, PARSE_HEADER, PARSE_BODY, PARSE_COMPLETE,
};

class Request : public HttpMessage {

 private:
  Request(const Request& other);
  Request& operator=(const Request& rhs);

  std::string raw_;
  enum ParseStatus status_;

  enum Method method_;
  std::string uri_;
  std::string http_version_;

 public:
  Request();
  ~Request();

  void AppendRawData(std::string raw);

  enum Method GetMethod() const;
  const std::string& GetURI() const;
  const std::string& GetVersion() const;

  class ParseBodyException : public std::runtime_error {
   public:
    ParseBodyException(const std::string& what);
  };

  class ParseHeaderException : public std::runtime_error {
   public:
    ParseHeaderException(const std::string& what);
  };

  class ParseStartlineException : public std::runtime_error {
   public:
    ParseStartlineException(const std::string& what);
  };

 private:
  void ParseRequest();
  size_t ParseStartline(size_t idx);
  size_t ParseHeader(size_t idx);
  size_t ParseBody(size_t idx);

};

#endif /* REQUEST_HPP */
