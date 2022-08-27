#include "wondermake-utility/Time.h"

#include <time.h>

Result<std::string> TimePointToISO8601(std::chrono::system_clock::time_point aTimePoint)
{
	try
	{
		return Ok(std::format("{:%FT%T%z}", aTimePoint));
	}
	catch (std::format_error)
	{
		return Err();
	}
}
