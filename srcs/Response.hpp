#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sstream>

#include "HttpMessage.hpp"

class Response : public HttpMessage {
 public:
  struct Status {
    std::map<int, std::string> code;
    Status();
  };
  static const Status kResponseStatus;

 private:
  std::string http_version_;
  int status_code_;
  std::string status_message_;

  Response(const Response& other);
  Response& operator=(const Response& rhs);

 public:
  Response();
  ~Response();

  void SetVersion(std::string version);
  void SetStatusCode(int status);
  void SetBody(std::string body);

  const std::string& GetVersion() const;
  int GetStatusCode() const;
  const std::string& GetStatusMessage() const;

  std::string Str() const;

  class StatusException : public std::domain_error {
   public:
    StatusException(const std::string& what);
  };

 private:
  void SetStatusMessage(std::string reason);

};

#endif /* RESPONSE_HPP */
