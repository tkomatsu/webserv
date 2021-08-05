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

}

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
