#include "main.hpp"

int main(int argc, char *argv[])
{
	WebServ webserv;
	(void)argc;

	if (!(argc == 1 || argc == 2))
	{
		std::cerr << "usage: ./webserv config_file" << std::endl;
		return (EXIT_FAILURE);
	}

	try {
		webserv.parse(argc == 2 ? argv[1] : WebServ::default_path);
		webserv.start();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return (EXIT_FAILURE);
}
