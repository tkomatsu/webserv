#include "config.hpp"

#include <algorithm>

#include "Client.hpp"

namespace {

std::string::const_iterator skip_leading_whitespace(
    std::string::const_iterator begin, std::string::const_iterator end) {
  return std::find_if_not(begin, end, ::isspace);
}

template <typename FirstType, typename SecondType>
std::string MapToString(const std::map<FirstType, SecondType>& map) {
  std::ostringstream oss;
  oss << "{";
  if (map.empty()) {
    oss << "}";
    return oss.str();
  }
  typename std::map<FirstType, SecondType>::const_iterator itr = map.begin();
  oss << itr->first << ": " << itr->second;
  while (++itr != map.end()) oss << ", " << itr->first << ": " << itr->second;
  oss << "}";
  return oss.str();
}

template <typename FirstType, typename SecondType>
std::string PairToString(const std::pair<FirstType, SecondType>& pair) {
  std::ostringstream oss;
  oss << "<" << pair.first << ", " << pair.second << ">";
  return oss.str();
}

template <typename T>
std::string VectorToString(const std::vector<T>& vector) {
  std::ostringstream oss;
  oss << "[";
  if (vector.empty()) {
    oss << "]";
    return oss.str();
  }
  typename std::vector<T>::const_iterator itr = vector.begin();
  oss << *itr;
  while (++itr != vector.end()) oss << ", " << *itr;
  oss << "]";
  return oss.str();
}

template <typename T>
std::string SetToString(const std::set<T>& set) {
  std::ostringstream oss;
  oss << "[";
  if (set.empty()) {
    oss << "]";
    return oss.str();
  }
  typename std::set<T>::const_iterator itr = set.begin();
  oss << *itr;
  while (++itr != set.end()) oss << ", " << *itr;
  oss << "]";
  return oss.str();
}

template <typename T>
void PrintKeyValue(const std::string& key, const T& value,
                   bool indent = false) {
  if (indent) std::cerr << "          ";
  std::cerr << std::setw(22) << std::left << key << ": " << value << std::endl;
}

bool IsInteger(const std::string& s) {
  if (s.empty()) return false;
  for (std::string::const_iterator itr = s.begin(); itr != s.end(); ++itr) {
    if (std::isdigit(*itr) == 0) return false;
  }
  return true;
}

bool IsExtension(const std::string& s) { return s == ".php" || s == ".py"; }

}  // namespace

namespace config {

Main::Main() {
  print_config = false;
  autoindex = false;
  client_max_body_size = 1000000;
}

Server::Server(const struct Main& main) {
  autoindex = main.autoindex;
  port = 80;
  client_max_body_size = main.client_max_body_size;
  host = "127.0.0.1";
  server_name = "";
  indexes = main.indexes;
  error_pages = main.error_pages;
}

Location::Location(const struct Server& server) {
  autoindex = server.autoindex;
  client_max_body_size = server.client_max_body_size;
  upload_pass = server.upload_pass;
  upload_store = server.upload_store;
  extensions = server.extensions;
  indexes = server.indexes;
  error_pages = server.error_pages;
  redirect = server.redirect;
}

Config::Config(const struct Server& server) : server_(server){};

Config::~Config(){};

Config::Config(const Config& other) : server_(other.server_) {}

Config& Config::operator=(const Config& other) {
  if (this != &other) {
    server_ = other.server_;
  }
  return *this;
}

int Config::GetPort() const { return server_.port; }

std::string Config::GetHost() const { return server_.host; }

std::string Config::GetServerName() const { return server_.server_name; }

const struct Location* Config::MatchLocation(const std::string& uri) const {
  if (server_.locations.empty())
    throw std::runtime_error("no location specified");
  const struct Location* longest_prefix = NULL;
  std::vector<const struct Location>::const_iterator itr;
  for (itr = server_.locations.begin(); itr != server_.locations.end(); ++itr) {
    if (uri.find(itr->path) == 0) {
      if (longest_prefix == NULL)
        longest_prefix = &*itr;
      else if (itr->path.length() > longest_prefix->path.length())
        longest_prefix = &*itr;
    }
  }
  if (longest_prefix == NULL)
    // throw Client::HttpResponseException("no matching location found");
    throw ft::HttpResponseException("404");
  return longest_prefix;
}

bool Config::GetAutoindex(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->autoindex;
}

int Config::GetClientMaxBodySize(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->client_max_body_size;
}

std::string Config::GetPath(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->path;
}

std::string Config::GetAlias(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->alias;
}

std::string Config::GetUploadPass(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->upload_pass;
}

std::string Config::GetUploadStore(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->upload_store;
}

std::set<std::string> Config::GetExtensions(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->extensions;
}

std::vector<std::string> Config::GetIndexes(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->indexes;
}

std::map<int, std::string> Config::GetErrorPages(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->error_pages;
}

std::set<enum Method> Config::GetAllowedMethods(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->allowed_methods;
}

std::pair<int, std::string> Config::GetRedirect(const std::string& uri) const {
  const struct Location* location = MatchLocation(uri);
  return location->redirect;
}

Parser::Parser(const std::string& filename) : filename_(filename) { Load(); }

Parser::~Parser() {}

std::vector<Config> Parser::GetConfigs() {
  std::vector<Config> configs;

  if (servers_.empty()) return configs;

  std::vector<struct Server>::const_iterator itr;
  for (itr = servers_.begin(); itr != servers_.end(); ++itr) {
    Config config = Config(*itr);
    configs.push_back(config);
  }

  return configs;
}

void Parser::Load() {
  std::ifstream file(filename_);
  if (!file.is_open()) {
    throw std::runtime_error("file could not be opened");
  }

  Directives mapping;
  mapping["print_config"] = &Parser::AddPrintConfig;
  mapping["server"] = &Parser::AddServer;
  mapping["location"] = &Parser::AddLocation;
  mapping["error_page"] = &Parser::AddErrorPage;
  mapping["client_max_body_size"] = &Parser::AddClientMaxBodySize;
  mapping["autoindex"] = &Parser::AddAutoindex;
  mapping["index"] = &Parser::AddIndex;
  mapping["listen"] = &Parser::AddListen;
  mapping["server_name"] = &Parser::AddServerName;
  mapping["return"] = &Parser::AddRedirect;
  mapping["allowed_methods"] = &Parser::AddAllowedMethods;
  mapping["alias"] = &Parser::AddAlias;
  mapping["upload_pass"] = &Parser::AddUploadPass;
  mapping["upload_store"] = &Parser::AddUploadStore;
  mapping["ext"] = &Parser::AddExtensions;

  enum Context context = MAIN;
  LineBuilder builder(file);
  LineComponent line;

  while (builder.GetNext(line)) {
    ValidateLineSyntax(line);

    Directives::iterator itr = mapping.find(line.name);
    if (itr == mapping.end()) {
      if (line.type != BLOCK_END)
        throw UnknownError(BuildError(line.name, "is not found"));
    } else {
      AddDirective f = itr->second;
      (this->*(f))(context, line.name, line.params);
    }

    if (line.type == BLOCK_START) PushContext(context);
    if (line.type == BLOCK_END) PopContext(context);
  }

  if (print_config_) Print();
}

void Parser::ValidateLineSyntax(const LineComponent& line) {
  if (line.type == NONE) {
    throw SyntaxError("directive should not be none");
  }
  if (line.type == BLOCK_START) {
    if (IsSimple(line.name))
      throw SyntaxError("directive does not have correct type");
    if (line.name.empty() && line.params.empty())
      throw SyntaxError("block start empty");
  }
  if (line.type == BLOCK_END) {
    if (!(line.name.empty() && line.params.empty()))
      throw SyntaxError("directives does not have correct type");
  }
  if (line.type == SIMPLE) {
    if (IsBlock(line.name))
      throw SyntaxError("directive does not have correct type");
    if (line.name.empty() && line.params.empty())
      throw SyntaxError("simple empty");
  }
}

void Parser::PushContext(enum Context& current) {
  if (current == MAIN)
    current = SERVER;
  else if (current == SERVER)
    current = LOCATION;
}

void Parser::PopContext(enum Context& current) {
  if (current == LOCATION)
    current = SERVER;
  else if (current == SERVER)
    current = MAIN;
}

bool Parser::IsBlock(const std::string& directive_name) {
  return directive_name == "server" || directive_name == "location";
}

bool Parser::IsSimple(const std::string& directive_name) {
  return !IsBlock(directive_name);
}

void Parser::AddPrintConfig(enum Context context, const std::string& name,
                            const std::vector<std::string>& params) {
  if (context != MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameter"));
  if (!(params.front() == "on" || params.front() == "off"))
    throw ParameterError(BuildError(name, "with invalid parameters"));

  print_config_ = params.front() == "on" ? true : false;
}

void Parser::AddServer(enum Context context, const std::string& name,
                       const std::vector<std::string>& params) {
  if (context != MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (!params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameter"));

  struct Server server(main_);
  servers_.push_back(server);
}

void Parser::AddLocation(enum Context context, const std::string& name,
                         const std::vector<std::string>& params) {
  if (context != SERVER)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameter"));
  assert(!servers_.empty());

  const std::string& path = params.front();
  struct Server& server = servers_.back();
  std::vector<struct Location>::const_iterator itr;
  for (itr = server.locations.begin(); itr != server.locations.end(); ++itr) {
    if (itr->path == path) throw ParameterError(BuildError(name, "duplicated"));
  }

  struct Location location(server);
  location.path = path;
  server.locations.push_back(location);
}

void Parser::AddErrorPage(enum Context context, const std::string& name,
                          const std::vector<std::string>& params) {
  if (params.size() < 2 || params.size() > 200)
    throw ParameterError(BuildError(name, "with wrong number of parameter"));

  std::string uri = params.back();
  std::vector<std::string>::const_iterator itr = params.begin();
  do {
    if (!IsInteger(*itr))
      throw ParameterError(BuildError(name, "string is not a number"));
    int code = std::atoi(itr->c_str());
    if (code < 300 || code > 599)
      throw std::domain_error("error_page code error");
    if (context == MAIN)
      main_.error_pages[code] = uri;
    else if (context == SERVER)
      servers_.back().error_pages[code] = uri;
    else if (context == LOCATION)
      servers_.back().locations.back().error_pages[code] = uri;
    ++itr;
  } while (itr + 1 != params.end());
}

void Parser::AddClientMaxBodySize(enum Context context, const std::string& name,
                                  const std::vector<std::string>& params) {
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  if (!IsInteger(params.front()))
    throw ParameterError(BuildError(name, "string is not a number"));
  int size = std::atoi(params.front().c_str());
  if (size < 0) throw ParameterError(BuildError(name, "cannot be negative"));
  if (context == MAIN)
    main_.client_max_body_size = size;
  else if (context == SERVER)
    servers_.back().client_max_body_size = size;
  else if (context == LOCATION)
    servers_.back().locations.back().client_max_body_size = size;
}

void Parser::AddAutoindex(enum Context context, const std::string& name,
                          const std::vector<std::string>& params) {
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));
  if (!(params.front() == "on" || params.front() == "off"))
    throw ParameterError(BuildError(name, "with invalid parameters"));

  bool autoindex = params.front() == "on" ? true : false;
  if (context == MAIN)
    main_.autoindex = autoindex;
  else if (context == SERVER)
    servers_.back().autoindex = autoindex;
  else if (context == LOCATION)
    servers_.back().locations.back().autoindex = autoindex;
}

void Parser::AddIndex(enum Context context, const std::string& name,
                      const std::vector<std::string>& params) {
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  for (std::vector<std::string>::const_reverse_iterator itr = params.rbegin();
       itr != params.rend(); ++itr) {
    if (itr->empty()) throw ParameterError(BuildError(name, "cannot be empty"));
    if (context == MAIN)
      main_.indexes.insert(main_.indexes.begin(), *itr);
    else if (context == SERVER)
      servers_.back().indexes.insert(servers_.back().indexes.begin(), *itr);
    else if (context == LOCATION)
      servers_.back().locations.back().indexes.insert(
          servers_.back().locations.back().indexes.begin(), *itr);
  }
}

void Parser::AddListen(enum Context context, const std::string& name,
                       const std::vector<std::string>& params) {
  if (context != SERVER)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  servers_.back().host = "127.0.0.1";
  servers_.back().port = std::atoi(params.front().c_str());
}

void Parser::AddServerName(enum Context context, const std::string& name,
                           const std::vector<std::string>& params) {
  if (context != SERVER)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  servers_.back().server_name = params.front();
}

void Parser::AddRedirect(enum Context context, const std::string& name,
                         const std::vector<std::string>& params) {
  if (context == MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 2)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  if (!IsInteger(params.front()))
    throw ParameterError(BuildError(name, "string is not a number"));
  int code = std::atoi(params.front().c_str());
  if (code < 300 || code > 599)
    throw std::domain_error("error_page code error");
  std::string uri = params.back();
  if (context == SERVER) {
    servers_.back().redirect = std::make_pair(code, uri);
  } else if (context == LOCATION) {
    servers_.back().locations.back().redirect = std::make_pair(code, uri);
  }
}

void Parser::AddAllowedMethods(enum Context context, const std::string& name,
                               const std::vector<std::string>& params) {
  if (context != LOCATION)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  for (std::vector<std::string>::const_iterator itr = params.begin();
       itr != params.end(); ++itr) {
    if (*itr == "GET")
      servers_.back().locations.back().allowed_methods.insert(GET);
    else if (*itr == "POST")
      servers_.back().locations.back().allowed_methods.insert(POST);
    else if (*itr == "DELETE")
      servers_.back().locations.back().allowed_methods.insert(DELETE);
    else
      throw ParameterError(BuildError(name, "invalid method name"));
  }
}

void Parser::AddAlias(enum Context context, const std::string& name,
                      const std::vector<std::string>& params) {
  if (context != LOCATION)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  servers_.back().locations.back().alias = params.front();
}

void Parser::AddUploadPass(enum Context context, const std::string& name,
                           const std::vector<std::string>& params) {
  if (context == MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  if (context == SERVER)
    servers_.back().upload_pass = params.front();
  else if (context == LOCATION)
    servers_.back().locations.back().upload_pass = params.front();
}

void Parser::AddUploadStore(enum Context context, const std::string& name,
                            const std::vector<std::string>& params) {
  if (context == MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  if (context == SERVER)
    servers_.back().upload_store = params.front();
  else if (context == LOCATION)
    servers_.back().locations.back().upload_store = params.front();
}

void Parser::AddExtensions(enum Context context, const std::string& name,
                           const std::vector<std::string>& params) {
  if (context == MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  for (std::vector<std::string>::const_iterator itr = params.begin();
       itr != params.end(); ++itr) {
    if (!IsExtension(*itr))
      throw ParameterError(BuildError(name, "is not valid"));
    if (context == SERVER)
      servers_.back().extensions.insert(*itr);
    else if (context == LOCATION)
      servers_.back().locations.back().extensions.insert(*itr);
  }
}

void Parser::Print() const {
  std::cerr << std::endl << "Print config      : on" << std::endl;
  std::cerr << "Number of servers : " << servers_.size() << std::endl;
  for (std::vector<struct Server>::const_iterator itr = servers_.begin();
       itr != servers_.end(); ++itr) {
    const Server& server = *itr;
    std::cerr << "-------------------------------------------------------------"
                 "-----------------"
              << std::endl;
    std::cerr << "                                 ~ Server ~" << std::endl;
    std::cerr << "-------------------------------------------------------------"
                 "-----------------";
    std::cerr << std::endl;
    PrintKeyValue("autoindex", server.autoindex);
    PrintKeyValue("port", server.port);
    PrintKeyValue("client_max_body_size", server.client_max_body_size);
    PrintKeyValue("host", server.host);
    PrintKeyValue("server_name", server.server_name);
    PrintKeyValue("index", VectorToString(server.indexes));
    PrintKeyValue("error_page", MapToString(server.error_pages));
    PrintKeyValue("redirect", PairToString(server.redirect));
    PrintKeyValue("upload_pass", server.upload_pass);
    PrintKeyValue("upload_store", server.upload_store);
    PrintKeyValue("extensions", SetToString(server.extensions));
    for (std::vector<struct config::Location>::const_iterator itr =
             server.locations.begin();
         itr != server.locations.end(); ++itr) {
      std::cerr << "-----------------------------------------------------------"
                   "-------------------"
                << std::endl;
      const struct config::Location& location = *itr;
      std::cerr << "Location: " << location.path << std::endl;
      PrintKeyValue("autoindex", location.autoindex, true);
      PrintKeyValue("client_max_body_size", location.client_max_body_size,
                    true);
      PrintKeyValue("index", VectorToString(location.indexes), true);
      PrintKeyValue("alias", location.alias, true);
      PrintKeyValue("error_page", MapToString(location.error_pages), true);
      PrintKeyValue("allowed_methods", SetToString(location.allowed_methods),
                    true);
      PrintKeyValue("redirect", PairToString(location.redirect), true);
      PrintKeyValue("upload_pass", location.upload_pass);
      PrintKeyValue("upload_store", location.upload_store);
      PrintKeyValue("extensions", SetToString(location.extensions));
    }
  }
}

LineBuilder::LineBuilder(std::ifstream& file) : file_(file){};

LineBuilder::~LineBuilder(){};

bool LineBuilder::GetNext(LineComponent& line) {
  line.name.clear();
  line.params.clear();
  line.type = NONE;

  do {
    if (current_.empty() && !std::getline(file_, current_)) break;
    Extract(line);
  } while (line.type == NONE);

  return IsLineFilled(line);
}

void LineBuilder::Extract(LineComponent& line) {
  std::string::const_iterator curr = current_.begin();
  std::string::const_iterator end = current_.end();

  if (line.name.empty()) curr = ExtractName(line.name, curr, end);
  curr = ExtractParams(line.params, curr, end);
  curr = ExtractType(line.type, curr, end);

  // [ \t]*(#(.*))?
  curr = skip_leading_whitespace(curr, end);
  if (curr == end || *curr == '#') {
    current_.clear();
  } else {
    current_ = std::string(curr, end);
  }
}

// [ \t]*([A-Za-z_][A-Za-z0-9_]*)?
std::string::const_iterator LineBuilder::ExtractName(
    std::string& name, std::string::const_iterator begin,
    std::string::const_iterator end) {
  std::string::const_iterator from;
  std::string::const_iterator to;

  from = skip_leading_whitespace(begin, end);
  if (from == end) return end;

  if (!std::isalpha(*from) && *from != '_') return from;

  to = from;
  while (std::isalnum(*to) || *to == '_') ++to;
  name = std::string(from, to);
  return to;
}

// [ \t]*([^{};#]*)
std::string::const_iterator LineBuilder::ExtractParams(
    std::vector<std::string>& params, std::string::const_iterator begin,
    std::string::const_iterator end) {
  std::string::const_iterator from;
  std::string::const_iterator to;

  from = skip_leading_whitespace(begin, end);
  if (from == end) return end;

  for (to = from; to != end; ++to)
    if (*to == '{' || *to == '}' || *to == ';' || (to == begin && *to == '#') ||
        (::isspace(*to) && *(to + 1) == '#'))
      break;
  std::string params_str = std::string(from, to);
  std::istringstream iss(params_str);
  std::string word;
  while (iss >> word) params.push_back(word);
  return to;
}

// ([{};]?)
std::string::const_iterator LineBuilder::ExtractType(
    LineType& type, std::string::const_iterator begin,
    std::string::const_iterator end) {
  // either {, }, ;, #, or end
  if (begin == end) {
    type = NONE;
    return end;
  }

  char token = *begin;
  type = token == ';'   ? SIMPLE
         : token == '{' ? BLOCK_START
         : token == '}' ? BLOCK_END
                        : NONE;

  if (token == ';' || token == '{' || token == '}') {
    return begin + 1;
  } else {
    return begin;
  }
}

bool LineBuilder::IsLineZeroWord(const LineComponent& line) {
  return line.name.empty() && line.params.empty();
}

bool LineBuilder::IsLineEmpty(const LineComponent& line) {
  return line.name.empty() && line.params.empty() && line.type == NONE;
}

bool LineBuilder::IsLineFilled(const LineComponent& line) {
  return !IsLineEmpty(line);
}

}  // namespace config
