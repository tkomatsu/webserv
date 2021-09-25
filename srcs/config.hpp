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

#include "utility.hpp"

namespace config {

enum Context {
  MAIN = 0,
  SERVER,
  LOCATION,
};

struct Main {
  Main();

  bool print_config;
  bool autoindex;
  int client_max_body_size;
  std::vector<std::string> indexes;
  std::map<int, std::string> error_pages;
};

struct Location;

struct Server {
  Server(const struct Main& main);

  bool autoindex;
  int port;
  int client_max_body_size;
  std::string host;
  std::string server_name;
  std::string upload_pass;
  std::string upload_store;
  std::set<std::string> extensions;
  std::vector<std::string> indexes;
  std::map<int, std::string> error_pages;
  std::pair<int, std::string> redirect;
  std::vector<struct Location> locations;
};

struct Location {
  Location(const struct Server& server);

  bool autoindex;
  int client_max_body_size;
  std::string path;
  std::string alias;
  std::string upload_pass;
  std::string upload_store;
  std::set<std::string> extensions;
  std::vector<std::string> indexes;
  std::map<int, std::string> error_pages;
  std::set<enum Method> allowed_methods;
  std::pair<int, std::string> redirect;
};

class Config {
 public:
  Config(const struct Server& server);
  ~Config();
  Config(const Config& other);
  Config& operator=(const Config& other);

  int GetPort() const;
  std::string GetHost() const;
  std::string GetServerName() const;

  bool GetAutoindex(const std::string& uri) const;
  int GetClientMaxBodySize(const std::string& uri) const;
  std::string GetPath(const std::string& uri) const;
  std::string GetAlias(const std::string& uri) const;
  std::string GetUploadPass(const std::string& uri) const;
  std::string GetUploadStore(const std::string& uri) const;
  std::set<std::string> GetExtensions(const std::string& uri) const;
  std::vector<std::string> GetIndexes(const std::string& uri) const;
  std::map<int, std::string> GetErrorPages(const std::string& uri) const;
  std::set<enum Method> GetAllowedMethods(const std::string& uri) const;
  std::pair<int, std::string> GetRedirect(const std::string& uri) const;
  bool HasLocation() const;
  std::map<int, std::string> GetMainAndServerErrorPages() const;

 private:
  struct Server server_;

  const struct Location* MatchLocation(const std::string& uri) const;
};

struct LineComponent;

class Parser {
 private:
  typedef void (Parser::*AddDirective)(enum Context, const std::string& name,
                                       const std::vector<std::string>& params);

  typedef std::map<std::string, AddDirective> Directives;

 public:
  explicit Parser(const std::string& filename);
  ~Parser();

  std::vector<Config> GetConfigs();
  void Print() const;

 private:
  /* prohibit copy constructor and assignment operator */
  Parser(const Parser& other);
  Parser& operator=(const Parser& other);

  bool print_config_;
  std::string filename_;
  struct Main main_;
  std::vector<struct Server> servers_;

  void Load();

  void ValidateLineSyntax(const LineComponent& line);

  void PushContext(enum Context& current);

  void PopContext(enum Context& current);

  bool IsBlock(const std::string& directive_name);

  bool IsSimple(const std::string& directive_name);

  void PrioritizeIndexes();

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
  void AddUploadPass(enum Context context, const std::string& name,
                     const std::vector<std::string>& params);
  void AddUploadStore(enum Context context, const std::string& name,
                      const std::vector<std::string>& params);
  void AddExtensions(enum Context context, const std::string& name,
                     const std::vector<std::string>& params);

 public:
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
    ContextError(const std::string& what) : std::runtime_error(what){};
  };
  class ParameterError : public std::runtime_error {
   public:
    ParameterError() : std::runtime_error("config: parameter error") {}
    ParameterError(const std::string& what) : std::runtime_error(what){};
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
  ~LineBuilder();

  bool GetNext(LineComponent& line);

 private:
  /* prohibit copy constructor and assignment operator */
  LineBuilder(const LineBuilder& other);
  LineBuilder& operator=(const LineBuilder& other);

  std::ifstream& file_;
  std::string current_;

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

  bool IsLineZeroWord(const LineComponent& line);
  bool IsLineEmpty(const LineComponent& line);
  bool IsLineFilled(const LineComponent& line);
};

}  // namespace config

#endif /* CONFIG_HPP */
