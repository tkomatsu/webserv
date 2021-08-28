#include "Response.hpp"

#include <sstream>

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

int Response::GetStatusCode() const { return status_code_; }

void Response::EndCGI() { status_ = DONE; }

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

void Response::EraseBody(ssize_t length) {
  body_.erase(body_.begin(), body_.begin() + length);
}

void Response::Clear() {
  HttpMessage::Clear();
  http_version_ = "1.1";
  SetStatusCode(200);
  AppendHeader("Content-Type", "");
  AppendHeader("Date", Now());
  AppendHeader("Server", "webserv/0.4.2");
}

void Response::SetStatusMessage(const std::string& msg) {
  status_message_ = msg;
}

void Response::ParseMessage() {
  delim_ = "\n";
  HttpMessage::ParseMessage();
}

void Response::ParseStartline() { HttpMessage::ParseStartline(); }

void Response::ParseHeader() { HttpMessage::ParseHeader(); }

void Response::ParseBody() {
  if (status_ == BODY) {
    AppendBody(raw_);
    raw_ = "";
  }
}

void Response::ErrorResponse(int status) {
  Clear();
  SetStatusCode(status);
  AppendBody(ErrorHtml(status));
  AppendHeader("Content-Type", "text/html");
  AppendHeader("Connection", "keep-alive");
  AppendHeader("Content-Length", ft::ltoa(body_.size()));
}

std::string Response::ErrorHtml(int status) {
  std::string body = "<html>\n<head><title>";
  body += ErrorStatusLine(status);
  body += "</title></head>\n<body>\n<center><h1>";
  body += ErrorStatusLine(status);
  body += "</h1></center>\n<hr><center>";
  body += GetHeader("Server");
  body += "</center>\n</body>\n</html>\n";
  return body;
}

std::string Response::ErrorStatusLine(int status) {
  std::stringstream s;
  s << status << " " << kResponseStatus.code.find(status)->second;
  return (s.str());
}

void Response::AutoIndexResponse(const std::string& path) {
  Clear();
  SetStatusCode(200);
  AppendHeader("Content-Type", "text/html");
  std::string body = AutoIndexHtml(path);
  if (body.empty())
    ErrorResponse(404);
  else
    AppendBody(body);
  AppendHeader("Content-Length", ft::ltoa(body_.size()));
}

void Response::DeleteResponse(void) {
  Clear();
  SetStatusCode(204);
  AppendHeader("Content-Type", "text/html");
  AppendHeader("Content-Length", "0");
}

void Response::RedirectResponse(int code, std::string location) {
  Clear();
  SetStatusCode(code);
  AppendHeader("Content-Type", "text/html");
  AppendHeader("Content-Length", "0");
  AppendHeader("Location", location);
}

std::string Response::AutoIndexHtml(const std::string& dir_path) {
  DIR* dirp = opendir(dir_path.c_str());
  if (dirp == NULL) return "";
  struct dirent* dp;
  std::vector<fileinfo> index;

  while ((dp = readdir(dirp)) != NULL) {
    fileinfo info;

    info.dirent_ = dp;
    stat((dir_path + std::string(dp->d_name)).c_str(), &info.stat_);
    index.push_back(info);
  }
  closedir(dirp);
  std::string tmp;
  tmp += "<html>\n<head><title>Index of ";
  tmp += "/";
  tmp += "</title></head>\n<body bgcolor=\"white\">\n<h1>Index of ";
  tmp += "/</h1><hr><pre><a href=\"../\">../</a>\n";

  for (std::vector<fileinfo>::iterator it = index.begin(); it != index.end();
       ++it) {
    fileinfo info = *it;

    if (info.dirent_->d_name[0] == '.') continue;

    tmp += "<a href=\"" + std::string(info.dirent_->d_name) + "/\">";
    if (std::string(info.dirent_->d_name).length() >= 50)
      tmp += std::string(info.dirent_->d_name).substr(0, 47) + "..&gt;";
    else {
      tmp += std::string(info.dirent_->d_name);
      if (S_ISDIR(info.stat_.st_mode)) tmp += "/";
    }
    tmp +=
        "</a>" +
        std::string(50 - std::min((size_t)50,
                                  std::string(info.dirent_->d_name).length()),
                    ' ');
    if (!S_ISDIR(info.stat_.st_mode)) tmp += " ";
    tmp += ft::AutoIndexNow(info.stat_.st_mtimespec.tv_sec);
    tmp += std::string(19, ' ');
    if (S_ISREG(info.stat_.st_mode))
      tmp += ft::ltoa(info.stat_.st_size) + "\n";
    else
      tmp += "-\n";
  }
  tmp += "</pre><hr></body>\n</html>\n";

  return tmp;
}
