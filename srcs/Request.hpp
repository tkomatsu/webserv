#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>
#include <vector>

#include "utility.hpp"

enum ParseStatus {
  PARSE_INIT,
  PARSE_STARTLINE,
  PARSE_HEADER,
  PARSE_BODY,
  PARSE_COMPLETE,
};

class Request {
 private:
  // enum Method method_;
  enum Method method_;
  std::string uri_;
  std::string http_version_;
  std::map<std::string, std::string> headers_;
  std::string body_;

  std::string raw_;
  enum ParseStatus status_;

 public:
  Request();
  Request(char* raw);  // これやるとセグる…
  ~Request();

  void AppendRawData(const char* raw);

  enum Method GetMethod() const;
  const std::string& GetURI() const;
  const std::string& GetVersion() const;
  const std::string& GetHeader(const std::string& key) const;
  std::string GetBody() const;

 private:
  void ParseRequest();
  void ParseStartline();
  void ParseHeader();
  void ParseBody();
};

#endif /* REQUEST_HPP */