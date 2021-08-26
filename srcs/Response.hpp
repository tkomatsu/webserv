#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <dirent.h>
#include <sys/stat.h>

#include <sstream>

#include "HttpMessage.hpp"

typedef struct fileinfo {
  struct dirent* dirent_;
  struct stat stat_;
} fileinfo;

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

  int GetStatusCode() const;
  const std::string& GetStatusMessage() const;

  void EndCGI();
  std::string Str() const;
  void EraseBody(ssize_t length);
  void Clear();
  void ErrorResponse(int status);
  void AutoIndexResponse(const std::string& path);

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

  std::string AutoIndexHtml(const std::string& path);
};

#endif /* RESPONSE_HPP */
