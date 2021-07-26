#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

}  // namespace ft