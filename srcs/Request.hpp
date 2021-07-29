#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <string>
#include <cstdlib>

#include "HttpMessage.hpp"

class Request : public HttpMessage {

 public:
  enum ParseStatus {
    STARTLINE,
    HEADER,
    BODY,
    DONE,
  };

 private:
  Request(const Request& other);
  Request& operator=(const Request& rhs);

  enum ParseStatus status_;

  enum Method method_;
  std::string uri_;

 public:
  Request();
  ~Request();

  void AppendRawData(std::string raw);

  enum Method GetMethod() const;
  const std::string& GetURI() const;
  enum ParseStatus GetStatus() const;

  // Fatal exception
  class RequestFatalException : public std::runtime_error {
   public:
    RequestFatalException(const std::string& what) : std::runtime_error(what) {}
  };

 private:
  void ParseMessage();
  void ParseStartline();
  void ParseHeader();
  void ParseBody();

};

#endif /* REQUEST_HPP */
