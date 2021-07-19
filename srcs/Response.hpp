#ifndef RESPONSE_HPP
#define RESPONSE_HPP

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

 public:
  Response();
  ~Response();

  void SetVersion(std::string);
  void SetStatusCode(int);
  void SetReason(std::string);
  void SetBody(std::string);
  void SetBody(const char* raw);  // キャストされるからいらない…？

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
  std::string http_version_;
  int status_code_;
  std::string status_message_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

#endif /* RESPONSE_HPP */