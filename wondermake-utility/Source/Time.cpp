#include "WonderMakeUtility/Time.h"

#include <time.h>

std::string TimePointToString(const std::chrono::system_clock::time_point& aTimePoint)
{
	const std::time_t rawtime = std::chrono::system_clock::to_time_t(aTimePoint);
	std::tm timeinfo;
	char buffer[80];

	localtime_s(&timeinfo, &rawtime);

	std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);

	return buffer;
}
