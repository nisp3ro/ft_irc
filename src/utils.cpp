#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>

/**
 * @brief Converts an IPv6 address from binary form to text form.
 *
 * This function takes a pointer to a 16-byte IPv6 address (or IPv6-mapped IPv4 address)
 * and converts it to its standard textual representation. If the address is an IPv6-mapped
 * IPv4 address (i.e. the first 12 bytes are zero except for the 0xFF values), it formats the
 * last 4 bytes as an IPv4 address.
 *
 * The function also attempts to compress the longest sequence of zeroes with "::" as per IPv6 notation.
 *
 * @param a0 Pointer to the binary representation of the IPv6 address.
 * @return std::string A string representing the IPv6 address in standard notation.
 */
std::string ft_inet_ntop6(const void *a0)
{
	const unsigned char *a = (const unsigned char *)a0;
	int i, j, max, best;
	char buf[INET6_ADDRSTRLEN];

	if (memcmp(a, "\0\0\0\0\0\0\0\0\0\0\377\377", 12))
		snprintf(buf, sizeof buf,
			"%x:%x:%x:%x:%x:%x:%x:%x",
			256*a[0]+a[1],256*a[2]+a[3],
			256*a[4]+a[5],256*a[6]+a[7],
			256*a[8]+a[9],256*a[10]+a[11],
			256*a[12]+a[13],256*a[14]+a[15]);
	else
		snprintf(buf, sizeof buf,
			"%x:%x:%x:%x:%x:%x:%d.%d.%d.%d",
			256*a[0]+a[1],256*a[2]+a[3],
			256*a[4]+a[5],256*a[6]+a[7],
			256*a[8]+a[9],256*a[10]+a[11],
			a[12],a[13],a[14],a[15]);
	/* Replace longest /(^0|:)[:0]{2,}/ with "::" */
	for (i=best=0, max=2; buf[i]; i++) {
		if (i && buf[i] != ':') continue;
		j = strspn(buf+i, ":0");
		if (j>max) best=i, max=j;
	}
	if (max>3) {
		buf[best] = buf[best+1] = ':';
		memmove(buf+best+2, buf+best+max, i-best-max+1);
	}
	return (buf);
}

/**
 * @brief Splits a string into a vector of substrings based on a delimiter.
 *
 * This function tokenizes the input string 'str' using the specified delimiter character 'c'.
 * It returns a vector containing each token as a separate string.
 *
 * @param str The input string to be split.
 * @param c The delimiter character used to split the string.
 * @return std::vector<std::string> A vector containing the substrings obtained after splitting.
 */
std::vector<std::string> ft_split(const std::string& str, char c)
{
	std::vector<std::string> strs;
	std::string part;
	std::istringstream part_stream(str);

	while (std::getline(part_stream, part, c))
	{
		strs.push_back(part);
	}
	return strs;
}

/**
 * @brief Generates a formatted date and time string.
 *
 * This function retrieves the current local time and formats it as a human-readable string
 * using the default locale's date and time representation. It uses the strftime() function
 * with the "%c" format specifier.
 *
 * @return std::string A string representing the current date and time.
 */
std::string dateString(void)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%c", timeinfo);
	return (buffer);
}

/**
 * @brief Converts an integer to a string.
 *
 * This function uses an output string stream to convert an integer value to its string representation.
 *
 * @param num The integer to convert.
 * @return std::string The string representation of the integer.
 */
std::string intToString(int num)
{
	std::ostringstream ss;
	ss << num;
	return (ss.str());
}

/**
 * @brief Checks if a string contains only digit characters.
 *
 * This function determines whether the input string 'str' is composed exclusively of digit characters (0-9).
 *
 * @param str The string to check.
 * @return bool Returns true if the string contains only digits, false otherwise.
 */
bool containsOnlyDigits(const std::string &str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}
