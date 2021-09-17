#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <algorithm>
#include <ctime>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "utility.hpp"

class HttpMessage {
 public:
  enum ParseStatus {
    STARTLINE,
    HEADER,
    BODY,
    DONE,
  };

  typedef std::map<std::string, std::string, ft::CaseInsensitiveCompare>
      http_headers;

  HttpMessage();
  ~HttpMessage();

  void AppendRawData(const char* data, size_t size);

  void SetVersion(const std::string& version);
  void AppendHeader(const std::string& key, const std::string& value);
  void AppendHeader(const std::pair<std::string, std::string>& pair);
  void AppendBody(std::vector<unsigned char>& data);
  void RemoveHeader(const std::string& key);

  const std::string& GetVersion() const;
  const http_headers& GetAllHeader() const;
  const std::string& GetHeader(const std::string& key) const;
  const std::vector<unsigned char>& GetBody() const;

  std::string Now(time_t time = std::time(NULL)) const;
  void Clear();

  /* Parse exception */
  class ParseBodyException : public std::runtime_error {
   public:
    ParseBodyException(const std::string& what) : std::runtime_error(what) {}
  };
  class ParseHeaderException : public std::runtime_error {
   public:
    ParseHeaderException(const std::string& what) : std::runtime_error(what) {}
  };
  class ParseStartlineException : public std::runtime_error {
   public:
    ParseStartlineException(const std::string& what)
        : std::runtime_error(what) {}
  };
  class HeaderKeyException : public std::invalid_argument {
   public:
    HeaderKeyException(const std::string& what) : std::invalid_argument(what) {}
  };

 protected:
  enum ParseStatus status_;
  std::string delim_;
  std::vector<unsigned char> raw_;
  std::string http_version_;
  http_headers headers_;
  std::vector<unsigned char> body_;

  virtual void ParseMessage();
  virtual void ParseStartline();
  virtual void ParseHeader();
  virtual void ParseBody();

 private:
  /* prohibit copy constructor and assignment operator */
  HttpMessage(const HttpMessage&);
  HttpMessage& operator=(const HttpMessage&);
};

#endif /* HTTPMESSAGE_HPP */
