#pragma once

#include <chrono>
#include <string>

#include "wondermake-utility/Result.h"

Result<std::string> TimePointToISO8601(std::chrono::system_clock::time_point aTimePoint);
