#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>

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

std::string intToString(int num)
{
	std::ostringstream ss;
	ss << num;
	return (ss.str());
}

bool containsOnlyDigits(const std::string &str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}
