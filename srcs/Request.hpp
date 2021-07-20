#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>

#include "utility.hpp"

enum ParseStatus {
  PARSE_INIT, PARSE_STARTLINE, PARSE_HEADER, PARSE_BODY, PARSE_COMPLETE,
};

class Request {

 private:
  Request(const Request& other);
  Request& operator=(const Request& rhs);

  std::string raw_;
  enum ParseStatus status_;

  enum Method method_;
  std::string uri_;
  std::string http_version_;
  std::map<std::string, std::string> headers_;
  std::string body_;

 public:
  Request();
  ~Request();

  void AppendRawData(std::string raw);

  enum Method GetMethod() const;
  const std::string& GetURI() const;
  const std::string& GetVersion() const;
  const std::string& GetHeader(const std::string& key) const;
  std::string GetBody() const;

 private:
  void ParseRequest();
  size_t ParseStartline(size_t idx);
  size_t ParseHeader(size_t idx);
  size_t ParseBody(size_t idx);

};

#endif /* REQUEST_HPP */
