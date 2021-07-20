#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "utility.hpp"

class Response {
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
  std::map<std::string, std::string> headers_;
  std::string body_;

  Response(const Response& other);
  Response& operator=(const Response& rhs);

 public:
  Response();
  ~Response();

  void SetVersion(std::string version);
  void SetStatusCode(int status);
  void SetBody(std::string body);

  void AppendHeader(std::string key, std::string value);
  void AppendHeader(std::pair<std::string, std::string> pair);
  void AppendBody(std::string str);

  const std::string& GetVersion() const;
  int GetStatusCode() const;
  const std::string& GetStatusMessage() const;
  const std::map<std::string, std::string>& GetAllHeader() const;
  const std::string& GetHeader(const std::string& key) const;
  const std::string& GetBody() const;

  std::string Str() const;

 private:
  void SetStatusMessage(std::string reason);

};

#endif /* RESPONSE_HPP */
