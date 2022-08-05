#pragma once

#include <chrono>
#include <string>

std::string TimePointToISO8601(const std::chrono::system_clock::time_point& aTimePoint);
