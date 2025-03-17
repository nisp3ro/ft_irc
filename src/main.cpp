#include <iostream>
#include "ft_irc.hpp"

/**
 * @brief Main entry point of the IRC server application.
 *
 * This program expects exactly two command-line arguments: the port number and the server password.
 * It performs the following steps:
 * 1. Validates the number of arguments; if incorrect, prints usage information and exits.
 * 2. Checks that the port argument contains only digits; if not, prints an error and exits.
 * 3. Instantiates a Server object with the provided port and password.
 * 4. Calls the server's listen() method to start the IRC server.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return int Returns 0 on successful execution, or 1 if an error occurs.
 */
int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}
	else if (!containsOnlyDigits(argv[1]))
	{
		std::cout << "Port must be a number" << std::endl;
		return (1);
	}
	
	Server server = Server(atoi(argv[1]), argv[2]);
	server.listen();
	return (0);
}
