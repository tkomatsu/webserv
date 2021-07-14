#ifndef UTILITY_HPP
#define UTILITY_HPP

enum Method {
  GET,
  POST,
  DELETE,
  INVALID,
};

char **ft_split(std::string s, char c);
std::string ft_inet_ntoa(struct in_addr in);
std::string ft_itoa(long num);

#endif /* UTILITY_HPP */
