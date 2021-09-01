#include <iostream>
#include <stdexcept>

#include "WebServ.hpp"

int main(int argc, char *argv[]) {
  if (!(argc == 1 || argc == 2)) {
    std::cerr << "usage: ./webserv [config_file]" << std::endl;
    return (EXIT_FAILURE);
  }

  try {
    WebServ webserv(argc == 2 ? argv[1] : WebServ::default_path_);

    webserv.Activate();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return (EXIT_FAILURE);
}
