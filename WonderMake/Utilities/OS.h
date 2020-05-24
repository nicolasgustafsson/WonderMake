#pragma once

#include <string>

std::string GetDateTime();

void SetThreadName(const std::string& aThreadName);

void SetThreadName(std::thread& aThread, const std::string& aThreadName);
