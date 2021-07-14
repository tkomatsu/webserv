#include "Response.hpp"

#include <iostream>
#include <sstream>

#include "utility.hpp"

std::string Response::GetResponse() const {
  std::string response;

  response = "HTTP/" + http_version_ + " " + ft_itoa(status_code_) + " " +
             reason_phrase_ + "\r\n";
  for (std::map<std::string, std::string>::const_iterator it = headers_.begin();
       it != headers_.end(); ++it) {
    response += it->first + ": " + it->second + "\r\n";
  }
  response += "\r\n";

  response += body_;

  return response;
}

void Response::SetVersion(std::string version) { http_version_ = version; }

void Response::SetStatus(int status) { status_code_ = status; }

void Response::SetReason(std::string reason) { reason_phrase_ = reason; }

void Response::SetHeader(std::string key, std::string value) {
  headers_[key] = value;
}

void Response::SetBody(std::string body) {
  SetHeader("Content-Length", ft_itoa(body.length()));
  body_ = body;
}
