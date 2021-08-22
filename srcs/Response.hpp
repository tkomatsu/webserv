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
  int status_code_;
  std::string status_message_;

  Response(const Response& other);
  Response& operator=(const Response& rhs);

 public:
  Response();
  ~Response();

  void SetStatusCode(int status);
  void SetBody(std::string body);

  int GetStatusCode() const;
  const std::string& GetStatusMessage() const;

  void EndCGI();
  std::string Str() const;
  void Clear();
  void ErrorResponse(int status);

  class StatusException : public std::domain_error {
   public:
    StatusException(const std::string& what) : std::domain_error(what) {}
  };

 private:
  void SetStatusMessage(const std::string& reason);
  virtual void ParseMessage();
  virtual void ParseStartline();
  virtual void ParseHeader();
  virtual void ParseBody();

  std::string ErrorHtml(int status);
  std::string ErrorStatusLine(int status);
};

#endif /* RESPONSE_HPP */
