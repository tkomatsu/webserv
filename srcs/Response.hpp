#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>
#include <vector>

class Response {
 public:
  std::string GetResponse() const;

  void SetVersion(std::string);
  void SetStatus(int);
  void SetReason(std::string);
  void SetHeader(std::string, std::string);
  void SetBody(std::string);

 private:
  std::string http_version_;
  int status_code_;
  std::string reason_phrase_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

#endif /* RESPONSE_HPP */