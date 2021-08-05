#include "config.hpp"

#if CONFIG_DEBUG
#include <iostream>
#define CONFIG_DEBUG_PRINT(x) std::cerr << x << std::endl;
#else
#define CONFIG_DEBUG_PRINT(x)
#endif


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
  while (++itr != map.end())
    oss << ", " << itr->first << ": " << itr->second;
  oss << "}";
  return oss.str();
}

template <typename T>
std::string VectorToString(const std::vector<T> &vector) {
  std::ostringstream oss;
  oss << "[";
  if (vector.empty()) {
      oss << "]";
      return oss.str();
  }
  typename std::vector<T>::const_iterator itr = vector.begin();
  oss << *itr;
  while (++itr != vector.end())
    oss << ", " << *itr;
  oss << "]";
  return oss.str();
}

}


namespace config {

template <typename T>
void PrintKeyValue(const std::string& key, const T& value, bool indent = false) {
  if (indent) std::cerr << "          ";
  std::cerr << std::setw(22) << std::left << key << ": " << value << std::endl;
}

bool isBlock(const std::string& directive_name) {
  // or check map
  if (directive_name == "server" || directive_name == "location")
    return true;
  return false;
}

bool isSimple(const std::string& directive_name) {
  // or check map
  return !isBlock(directive_name);
}

Config::Config(const std::string& filename) : filename_(filename) {
  Load();
}

Config::~Config() {}

void Config::Print() const {
  std::cerr << std::endl << "Print config      : on" << std::endl;
  std::cerr << "Number of servers : " << servers_.size() << std::endl;
  for (std::vector<struct Server>::const_iterator itr = servers_.begin();
       itr != servers_.end(); ++itr) {
    const Server& server = *itr;
    std::cerr << "------------------------------------------------------------------------------" << std::endl;
    std::cerr << "                                ~ Server " << server.id << " ~"<< std::endl;
    std::cerr << "------------------------------------------------------------------------------";
    std::cerr << std::endl;
    PrintKeyValue("autoindex", server.autoindex);
    PrintKeyValue("port", server.port);
    PrintKeyValue("client_max_body_size", server.client_max_body_size);
    PrintKeyValue("host", server.host);
    PrintKeyValue("server_name", server.server_name);
    PrintKeyValue("index", VectorToString(server.indexes));
    PrintKeyValue("error_page", MapToString(server.error_pages));
    PrintKeyValue("redirect", "NOT IMPLEMENTED");
    for (std::vector<struct config::Location>::const_iterator itr =
             server.locations.begin();
         itr != server.locations.end(); ++itr) {
      std::cerr << "------------------------------------------------------------------------------" << std::endl;
      const struct config::Location& location = *itr;
      std::cerr << "Location: " << location.path << std::endl;
      PrintKeyValue("autoindex", location.autoindex, true);
      PrintKeyValue("client_max_body_size", location.client_max_body_size, true);
      PrintKeyValue("index", VectorToString(location.indexes), true);
      PrintKeyValue("alias", location.alias, true);
      PrintKeyValue("error_page", MapToString(location.error_pages), true);
      PrintKeyValue("allowed_methods", VectorToString(location.allowed_methods),
                    true);
      PrintKeyValue("redirect", "NOT IMPLEMENTED", true);
    }
  }
}

void Config::ValidateLineSyntax(const LineComponent& line) {
  if (line.type == NONE) {
    throw SyntaxError("directive should not be none");
  }
  if (line.type == BLOCK_START) {
    if (isSimple(line.name))
      throw SyntaxError("directive does not have correct type");
    if (line.name.empty() && line.params.empty())
      throw SyntaxError("block start empty");
  }
  if (line.type == BLOCK_END) {
    if (!(line.name.empty() && line.params.empty()))
      throw SyntaxError("directives does not have correct type");
  }
  if (line.type == SIMPLE) {
    if (isBlock(line.name))
      throw SyntaxError("directive does not have correct type");
    if (line.name.empty() && line.params.empty())
      throw SyntaxError("simple empty");
  }
}

void Config::Load() {
  std::ifstream file(filename_);
  if (!file.is_open()) {
    throw std::runtime_error("file could not be opened");
  }

  Directives mapping;
  mapping["print_config"] = &Config::AddPrintConfig;
  mapping["server"] = &Config::AddServer;
  mapping["location"] = &Config::AddLocation;
  mapping["error_page"] = &Config::AddErrorPage;
  mapping["client_max_body_size"] = &Config::AddClientMaxBodySize;
  mapping["autoindex"] = &Config::AddAutoindex;
  mapping["index"] = &Config::AddIndex;
  mapping["listen"] = &Config::AddListen;
  mapping["server_name"] = &Config::AddServerName;
  mapping["redirect"] = &Config::AddRedirect;
  mapping["allowed_methods"] = &Config::AddAllowedMethods;
  mapping["alias"] = &Config::AddAlias;

  enum Context context = MAIN;
  LineBuilder builder(file);
  LineComponent line;

  while (builder.GetNext(line)) {
    ValidateLineSyntax(line);

    const std::string& function = line.name;
    Directives::iterator itr = mapping.find(function);
    if (itr == mapping.end()) {
      if (line.type != BLOCK_END)
        throw UnknownError(BuildError(line.name, "is not found"));
    } else {
      AddDirectiveFunc f = itr->second;
      (this->*(f))(context, line.name, line.params);
    }

    // bad
    if (line.type == BLOCK_START) {
      if (context == MAIN)
        context = SERVER;
      else if (context == SERVER)
        context = LOCATION;
    } else if (line.type == BLOCK_END) {
      if (context == LOCATION)
        context = SERVER;
      else if (context == SERVER)
        context = MAIN;
    }
  }

  // TODO: set defaults

  if (print_config_) Print();
}

void Config::AddPrintConfig(enum Context context, const std::string& name,
                            const std::vector<std::string>& params) {
  if (context != MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameter"));

  if (params.front() == "on")
    print_config_ = true;
  else if (params.front() == "off")
    print_config_ = false;
  else
    throw ParameterError(BuildError(name, "with invalid parameter"));
}

void Config::AddServer(enum Context context, const std::string& name,
    const std::vector<std::string>& params) {
  if (context != MAIN)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (!params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameter"));

  struct Server server;
  server.id = servers_.size();
  servers_.push_back(server);
}

void Config::AddLocation(enum Context context, const std::string& name,
    const std::vector<std::string>& params) {
  if (context != SERVER)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameter"));

  struct Location location;
  location.path = params.front();
  assert(!servers_.empty()); // assert
  servers_.back().locations.push_back(location);
}

void Config::AddErrorPage(enum Context context, const std::string& name,
    const std::vector<std::string>& params) {
  if (params.size() < 2 || params.size() > 200)
    throw ParameterError(BuildError(name, "with wrong number of parameter"));

  std::string uri = params.back();
  std::vector<std::string>::const_iterator itr = params.begin();
  do {
    // TODO: check string
    int code = std::atoi((*itr).c_str());
    if (code < 400 || code > 599)
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

void Config::AddClientMaxBodySize(enum Context context, const std::string& name,
    const std::vector<std::string>& params) {
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  int size = std::atoi(params.front().c_str());
  // TODO: validate
  if (context == MAIN)
    main_.client_max_body_size = size;
  else if (context == SERVER)
    servers_.back().client_max_body_size = size;
  else if (context == LOCATION)
    servers_.back().locations.back().client_max_body_size = size;
}


void Config::AddAutoindex(enum Context context, const std::string& name,
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

void Config::AddIndex(enum Context context, const std::string& name,
    const std::vector<std::string>& params) {
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  for (std::vector<std::string>::const_iterator itr = params.begin(); itr != params.end(); ++itr) {
    // TODO: check string, empty, append if exist
    if (context == MAIN)
      main_.indexes.push_back(*itr);
    else if (context == SERVER)
      servers_.back().indexes.push_back(*itr);
    else if (context == LOCATION)
      servers_.back().locations.back().indexes.push_back(*itr);
  }
}

void Config::AddListen(enum Context context, const std::string& name,
                       const std::vector<std::string>& params) {
  if (context != SERVER)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  // TODO: implement
  servers_.back().host = "127.0.0.1";
  servers_.back().port = std::atoi(params.front().c_str());
}

void Config::AddServerName(enum Context context, const std::string& name,
                           const std::vector<std::string>& params) {
  if (context != SERVER)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  // TODO: check string
  servers_.back().server_name = params.front();
}

void Config::AddRedirect(enum Context context, const std::string& name,
                         const std::vector<std::string>& params) {
  (void)context;
  (void)name;
  (void)params;
  // TODO: implement
  // throw std::runtime_error("redirect is not implemented yet");
}

void Config::AddAllowedMethods(enum Context context, const std::string& name,
                               const std::vector<std::string>& params) {
  if (context != LOCATION)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.empty())
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  for (std::vector<std::string>::const_iterator itr = params.begin(); itr != params.end(); ++itr) {
    // TODO: check string
    servers_.back().locations.back().allowed_methods.push_back(*itr);
  }
}

void Config::AddAlias(enum Context context, const std::string& name,
    const std::vector<std::string>& params) {
  if (context != LOCATION)
    throw ContextError(BuildError(name, "is not allowed here"));
  if (params.size() != 1)
    throw ParameterError(BuildError(name, "with wrong number of parameters"));

  // TODO: check string
  servers_.back().locations.back().alias = params.front();
}

}  // namespace config


namespace config {

bool isLineZeroWord(const LineComponent& line) {
  return line.name.empty() && line.params.empty();
}

bool isLineEmpty(const LineComponent& line) {
  return line.name.empty() && line.params.empty() && line.type == NONE;
}

bool isLineFilled(const LineComponent& line) {
  return !isLineEmpty(line);
}


LineBuilder::LineBuilder(std::ifstream& file) : file_(file) {};

LineBuilder::~LineBuilder() {};

bool LineBuilder::GetNext(LineComponent& line) {
  // should not be here
  line.name.clear();
  line.params.clear();
  line.type = NONE;

  do {
    // if (current_.empty() && !std::getline(file_, current_))
    if (current_.empty()) {
      if (std::getline(file_, current_)) {
        ++line_num_;
      } else {
        break;
      }
    }
    Extract(line);
  } while (line.type == NONE);

  return isLineFilled(line);
}

// TODO: document/comment on line structure
void LineBuilder::Extract(LineComponent& line) {
  std::string::const_iterator curr = current_.begin();
  std::string::const_iterator end = current_.end();

  if (line.name.empty()) // unclear
    curr = ExtractName(line.name, curr, end);
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
// TODO: throw if name is not empty?
std::string::const_iterator LineBuilder::ExtractName(
    std::string& name, std::string::const_iterator begin,
    std::string::const_iterator end) {
  std::string::const_iterator from;
  std::string::const_iterator to;

  from = skip_leading_whitespace(begin, end);
  if (from == end)
    return end;

  if (!std::isalpha(*from) && *from != '_')
    return from;

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
  if (from == end)
    return end;

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

}  // namespace config
