#include "stdafx.h"

#include "Utilities/OS.h"

#include <time.h>

void GetDateTime(std::string& aOutString)
{
	std::time_t rawtime;
	std::tm timeinfo;
	char buffer[80];

	std::time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	std::strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S", &timeinfo);
	aOutString = buffer;
}
