#pragma once
#include <string>

namespace Utility
{
	std::string Utf8Encode(const std::wstring& aWideString);
	std::wstring Utf8Decode(const std::string& aUtf8String);
}