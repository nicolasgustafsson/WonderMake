#include "wondermake-utility/Time.h"

#include <time.h>

std::string TimePointToISO8601(const std::chrono::system_clock::time_point& aTimePoint)
{
	try
	{
		return std::format("{:%FT%T%z}", aTimePoint);
	}
	catch (std::format_error)
	{
		return "0000-00-00T00:00:00+0000";
	}
}
