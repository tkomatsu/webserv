#ifndef RESPONSE_HPP
#define RESPONSE_HPP

/* <<<<<<< HEAD
#include <map>
#include <string>
#include <vector>

struct ResponseStatus {
  std::map<int, std::string> code;
  ResponseStatus();
};

class Response {
 public:
  static const ResponseStatus kResponseStatus;
======= */
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
  int status_code_;
  std::string status_message_;

  Response(const Response& other);
  Response& operator=(const Response& rhs);

 public:
  Response();
  ~Response();

  void SetStatusCode(int status);
  void SetReason(std::string);
  void SetBody(std::string body);

  int GetStatusCode() const;
  const std::string& GetStatusMessage() const;

  std::string Str() const;

  class StatusException : public std::domain_error {
   public:
    StatusException(const std::string& what) : std::domain_error(what) {}
  };

 private:
  void SetStatusMessage(std::string reason);

};

#endif /* RESPONSE_HPP */
