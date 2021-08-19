#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "HttpMessage.hpp"

class Request : public HttpMessage {
 private:
  Request(const Request& other);
  Request& operator=(const Request& rhs);

  enum Method method_;
  std::string uri_;

 public:
  Request();
  ~Request();

  enum Method GetMethod() const;
  const std::string& GetURI() const;
  enum ParseStatus GetStatus() const;

  void EraseBody(ssize_t length);
  void Clear();

  // Fatal exception
  class RequestFatalException : public std::runtime_error {
   public:
    RequestFatalException(const std::string& what) : std::runtime_error(what) {}
  };

 private:
  virtual void ParseMessage();
  virtual void ParseStartline();
  virtual void ParseHeader();
  virtual void ParseBody();
};

#endif /* REQUEST_HPP */
