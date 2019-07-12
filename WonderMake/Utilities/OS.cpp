#include "pch.h"

#include "Utilities/OS.h"

#include <time.h>

std::string GetDateTime()
{
	std::time_t rawtime;
	std::tm timeinfo;
	char buffer[80];

	std::time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", &timeinfo);
	return buffer;
}
