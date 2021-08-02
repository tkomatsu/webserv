#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctime>
#include <sstream>
#include <vector>

namespace ft {

namespace {

int count(std::string s, char c) {
  int word = 0, len = 0, i = 0;

  while (s[i]) {
    if (s[i] == c) {
      if (len > 0) word++;
      len = 0;
    } else
      len++;
    ++i;
  }
  if (len > 0) word++;
  return (word);
}

}  // namespace

char **split(std::string s, char c) {
  char **p;
  int i = 0, j = 0, cnt;

  if (!(p = (char **)malloc(sizeof(char *) * (count(s, c) + 1)))) return (NULL);
  while (s[j]) {
    while (s[j] == c) j++;
    cnt = j;
    while (s[j] != c && s[j]) j++;
    if (j > cnt) p[i++] = strdup(s.substr(cnt, j - cnt).c_str());
  }
  p[i] = NULL;
  return (p);
}

std::string inet_ntoa(struct in_addr in) {
  char buffer[18];
  std::string ret;

  unsigned char *bytes = (unsigned char *)&in;
  sprintf(buffer, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
  ret = buffer;
  return ret;
}

std::string ltoa(long num) {
  std::stringstream out;
  out << num;
  std::string ret = out.str();

  return ret;
}

std::string trim(std::string s, std::string chars) {
  std::string::size_type pos = s.find_first_not_of(chars);
  if (pos == std::string::npos) return "";
  std::string::size_type endpos = s.find_last_not_of(chars);
  std::string ret = s.substr(pos, endpos - pos + 1);
  return ret;
}

std::vector<std::string> vsplit(std::string s, char delim) {
  std::vector<std::string> v;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    v.push_back(item);
  }
  return v;
}

std::pair<std::string, std::string> div(std::string s, char delim) {
  std::string k = s.substr(0, s.find(delim));
  std::string v = s.substr(s.find(delim) + 1);
  return std::make_pair(k, v);
}

std::string AutoIndexNow(time_t time) {
  char buf[80];
  std::strftime(buf, sizeof(buf), "%d-%b-%Y %R", std::gmtime(&time));
  return std::string(buf);
}

}  // namespace ft
