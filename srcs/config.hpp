#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
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

enum Context {
  MAIN = 0,
  SERVER,
  LOCATION,
};

struct Main {
  bool print_confg;
  bool autoindex;
  int client_max_body_size;
  std::vector<std::string> indexes;
  std::map<int, std::string> error_pages;
};

struct Location;

struct Server {
  bool autoindex;
  int id;
  int port;
  int client_max_body_size;
  std::string host;
  std::string server_name;
  std::vector<std::string> indexes;
  std::map<int, std::string> error_pages;
  std::map<int, std::string> redirects;
  std::vector<struct Location> locations;
};

struct Location {
  bool autoindex;
  int client_max_body_size;
  std::string path;
  std::string alias;
  std::vector<std::string> indexes;
  std::map<int, std::string> error_pages;
  std::vector<std::string> allowed_methods; // TODO: to enum
  std::map<int, std::string> redirects;
};

struct LineComponent;

class Config {
 private:
  typedef void (Config::*AddDirectiveFunc)(
      enum Context,
      const std::string& name,
      const std::vector<std::string>& params
  );

  typedef std::map<std::string, AddDirectiveFunc> Directives;

 public:
  explicit Config(const std::string& filename); // TODO: move to webserv
  ~Config();

  void Load(); // or call on constructor
  void Print() const;

 private:
  bool print_config_;
  std::string filename_;
  std::vector<struct Server> servers_;

  void ValidateLineSyntax(const LineComponent& line); // TODO: remove dep

  void AddPrintConfig(enum Context context, const std::string& name,
                      const std::vector<std::string>& params);
  void AddServer(enum Context context, const std::string& name,
                 const std::vector<std::string>& params);
  void AddLocation(enum Context context, const std::string& name,
                   const std::vector<std::string>& params);
  void AddErrorPage(enum Context context, const std::string& name,
                    const std::vector<std::string>& params);
  void AddClientMaxBodySize(enum Context context, const std::string& name,
                            const std::vector<std::string>& params);
  void AddAutoindex(enum Context context, const std::string& name,
                    const std::vector<std::string>& params);
  void AddIndex(enum Context context, const std::string& name,
                const std::vector<std::string>& params);
  void AddListen(enum Context context, const std::string& name,
                 const std::vector<std::string>& params);
  void AddServerName(enum Context context, const std::string& name,
                     const std::vector<std::string>& params);
  void AddRedirect(enum Context context, const std::string& name,
                   const std::vector<std::string>& params);
  void AddAllowedMethods(enum Context context, const std::string& name,
                         const std::vector<std::string>& params);
  void AddAlias(enum Context context, const std::string& name,
                const std::vector<std::string>& params);

 public:
  // temporary
  static std::string BuildError(const std::string& directive_name,
                                const std::string& message, size_t line = 0) {
    (void)line;
    std::ostringstream oss;
    oss << "config: ";
    oss << "\"" << directive_name << "\" directive ";
    oss << message;
    return oss.str();
  }

  class SyntaxError : public std::runtime_error {
   public:
    SyntaxError() : std::runtime_error("config: syntax error") {}
    SyntaxError(const std::string& what) : std::runtime_error(what){};
  };
  class UnknownError : public std::runtime_error {
   public:
    UnknownError() : std::runtime_error("config: unknown error") {}
    UnknownError(const std::string& what) : std::runtime_error(what){};
  };
  class ContextError : public std::runtime_error {
   public:
    ContextError() : std::runtime_error("config: context error") {}
    ContextError(const std::string& what) : std::runtime_error(what) {};
  };
  class ParameterError : public std::runtime_error {
   public:
    ParameterError() : std::runtime_error("config: parameter error") {}
    ParameterError(const std::string& what) : std::runtime_error(what) {};
  };
};

enum LineType {
  SIMPLE = 0,
  BLOCK_START,
  BLOCK_END,
  NONE,
};

struct LineComponent {
  enum LineType type;
  std::string name;
  std::vector<std::string> params;
};

class LineBuilder {
 public:
  explicit LineBuilder(std::ifstream& file);
  virtual ~LineBuilder();

  bool GetNext(LineComponent& line);
  bool GetNext(LineComponent& line, std::ifstream& file);

 private:
  std::ifstream& file_;
  std::string current_;
  std::string::size_type line_num_;

  void Extract(LineComponent& line);

  std::string::const_iterator ExtractName(std::string& name,
                                          std::string::const_iterator begin,
                                          std::string::const_iterator end);
  std::string::const_iterator ExtractParams(std::vector<std::string>& params,
                                            std::string::const_iterator begin,
                                            std::string::const_iterator end);
  std::string::const_iterator ExtractType(LineType& type,
                                          std::string::const_iterator begin,
                                          std::string::const_iterator end);
};

}  // namespace config

#endif /* CONFIG_HPP */
