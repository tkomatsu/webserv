#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <ctime>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
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

 protected:
  typedef std::map<std::string, std::string, ft::CaseInsensitiveCompare>
      http_header;

  enum ParseStatus status_;
  std::string delim_;
  std::string raw_;
  std::string http_version_;
  http_header headers_;
  std::vector<unsigned char> body_;

  virtual void ParseMessage();
  virtual void ParseStartline();
  virtual void ParseHeader();
  virtual void ParseBody();

 public:
  HttpMessage();
  ~HttpMessage();

  void AppendRawData(const std::string& raw);

  void SetVersion(const std::string& version);
  void AppendHeader(const std::string& key, const std::string& value);
  void AppendHeader(const std::pair<std::string, std::string>& pair);
  void AppendBody(const std::string&);
  void RemoveHeader(const std::string& key);

  const std::string& GetVersion() const;
  const http_header& GetAllHeader() const;
  const std::string& GetHeader(const std::string& key) const;
  const std::vector<unsigned char>& GetBody() const;

  std::string Now(time_t time = std::time(NULL)) const;
  void Clear();

  // Parse exception
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

 private:
  /* prohibit copy constructor and assignment operator */
  HttpMessage(const HttpMessage&);
  HttpMessage& operator=(const HttpMessage&);
};

#endif /* HTTPMESSAGE_HPP */
