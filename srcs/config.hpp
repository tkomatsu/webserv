#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cctype>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace config {

static std::string default_path = "./default.conf";

// Allowed directives
//   (in the order of subject requirements)
//   (change to set or ordered vector)
// Context: MAIN
static std::string main_directives[] = {
    "#",
    "server",
    "error_page",
    "client_max_body_size",
    "autoindex",
    "index",
};

// Context: SERVER
static std::string server_directives[] = {
    "#",
    "listen",
    "server_name",
    "error_page",
    "client_max_body_size",
    "location",
    "redirect",
    "autoindex",
    "index",
};

// Context: LOCATION
static std::string location_directives[] = {
    "#",
    "error_page",
    "client_max_body_size",
    "allowed_methods",
    "redirect",
    "alias",
    "autoindex",
    "index",
    // add upload and cgi
};

enum Context {
  MAIN = 0,
  SERVER,
  LOCATION,
};

enum Type {
  SIMPLE = 0,
  BLOCK_START,
  BLOCK_END,
  NONE,
};

struct Directive {
  enum Context context;
  enum Type type;
  std::string name;
  std::vector<std::string> params;
};

struct Location;

struct Server {
  bool autoindex;
  int port;
  int client_max_body_size;
  std::string host;
  std::string server_name;
  std::string index;
  std::map<int, std::string> error_pages;
  std::map<int, std::string> redirects;
  std::vector<struct Location> locations;
};

struct Location {
  bool autoindex;
  int client_max_body_size;
  std::string path;
  std::string index;
  std::string alias;
  std::map<int, std::string> error_pages;
  std::vector<std::string> allowed_methods; // to enum
  std::map<int, std::string> redirects;
};

static std::string methods[] = {
    "GET", "PUT", "HEAD", "POST", "DELETE", "CONNECT", "OPTIONS", "TRACE"
};

class Config {
 public:
  explicit Config(const std::string& filename);
  ~Config();

  void Load(); // or call on constructor

 private:
  std::string filename_;
  std::vector<struct Server> servers_;
};

}  // namespace config

#endif /* CONFIG_HPP */
