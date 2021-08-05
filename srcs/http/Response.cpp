#include "Response.hpp"

Response::Status::Status() {
  code[100] = "Continue";
  code[101] = "Swithing Protocol";
  code[102] = "Processing";
  code[103] = "Early Hints";
  code[200] = "OK";
  code[201] = "Created";
  code[202] = "Accepted";
  code[203] = "Non-Authoritative Information";
  code[204] = "No Content";
  code[205] = "Reset Content";
  code[206] = "Partial Content";
  code[300] = "Multiple Choice";
  code[301] = "Moved Permanently";
  code[302] = "Found";
  code[303] = "See Other";
  code[304] = "Not Modified";
  code[307] = "Temporary Redirect";
  code[308] = "Permanent Redirect";
  code[400] = "Bad Request";
  code[401] = "Unauthorized";
  code[402] = "Payment Required";
  code[403] = "Forbidden";
  code[404] = "Not Found";
  code[405] = "Method Not Allowd";
  code[406] = "Not Acceptable";
  code[407] = "Proxy Authentication Required";
  code[408] = "Request Timeout";
  code[409] = "Conflict";
  code[410] = "Gone";
  code[411] = "Length Required";
  code[412] = "Precondition Failed";
  code[413] = "Payload Too Large";
  code[414] = "URI Too Long";
  code[415] = "Unsupported Media Type";
  code[416] = "Range Not Satisfiable";
  code[417] = "Expectation Failed";
  code[418] = "I'm a teapot";
  code[421] = "Misdirected Request";
  code[425] = "Too Early";
  code[426] = "Upgrade Required";
  code[428] = "Precondition Required";
  code[429] = "Too Many Requests";
  code[431] = "Request Header Fields Too Large";
  code[451] = "Unavailable For Legal Reasons";
  code[500] = "Internal Server Error";
  code[501] = "Not Implemented";
  code[502] = "Bad Gateway";
  code[503] = "Service Unavailable";
  code[504] = "Gateway Timeout";
  code[505] = "HTTP Version Not Supported";
  code[506] = "Variant, Also Negotiates";
  code[510] = "Not Extended";
  code[511] = "Network Authentication Required";
};

const Response::Status Response::kResponseStatus = Status();

Response::Response() {
  http_version_ = "1.1";
  SetStatusCode(200);
  AppendHeader("Content-Type", "");
  AppendHeader("Date", Now());
  AppendHeader("Server", "webserv");
}

Response::~Response() {}

void Response::SetStatusCode(int status) {
  if (kResponseStatus.code.find(status) == kResponseStatus.code.end())
    throw StatusException("Invalid status code");
  status_code_ = status;
  SetStatusMessage(kResponseStatus.code.find(status)->second);
}

void Response::SetBody(std::string body) { body_ = body; }

int Response::GetStatusCode() const { return status_code_; }

const std::string& Response::GetStatusMessage() const {
  return status_message_;
}

std::string Response::Str() const {
  std::stringstream s;

  s << "HTTP/" << http_version_ << " " << status_code_ << " " << status_message_
    << "\r\n";

  for (std::map<std::string, std::string>::const_iterator i = headers_.begin();
       i != headers_.end(); ++i) {
    s << i->first << ": " << i->second << "\r\n";
  }
  s << "\r\n" << body_;
  return s.str();
}

void Response::Clear() {
  HttpMessage::Clear();
  http_version_ = "1.1";
  SetStatusCode(200);
  AppendHeader("Content-Type", "");
  AppendHeader("Date", Now());
  AppendHeader("Server", "webserv");
}

void Response::SetStatusMessage(const std::string& msg) {
  status_message_ = msg;
}

void Response::ParseStartline() { HttpMessage::ParseStartline(); }

void Response::ParseHeader() { HttpMessage::ParseHeader(); }

void Response::ParseMessage() { HttpMessage::ParseMessage(); }

void Response::ParseBody() {
  if (status_ == BODY) {
    AppendBody(raw_);
    status_ = DONE;
  }
}
