#include "pch.h"

#include "Utilities/OS.h"

#include <processthreadsapi.h>
#include <string>
#include <time.h>
#include <windows.h>

std::wstring LocStringToWString(const std::string& aString)
{
	const auto wCharLength = MultiByteToWideChar(CP_UTF8, 0, aString.c_str(), -1, NULL, 0);

	std::vector<wchar_t> buffer(wCharLength, L'\0');
	
	MultiByteToWideChar(CP_UTF8, 0, aString.c_str(), -1, buffer.data(), wCharLength);
	
	return std::wstring(buffer.cbegin(), buffer.cend());
}

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

void LocSetThreadName(const HANDLE aThreadHandle, const std::string& aThreadName)
{
	const auto wName = LocStringToWString(aThreadName);

	const HRESULT hResult = SetThreadDescription(aThreadHandle, wName.c_str());

	if (FAILED(hResult))
	{
		WmLog(TagError, "Failed to set thread name: ", aThreadName);
	}
}

void SetThreadName(const std::string& aThreadName)
{
	LocSetThreadName(GetCurrentThread(), aThreadName);
}

void SetThreadName(std::thread& aThread, const std::string& aThreadName)
{
	LocSetThreadName(aThread.native_handle(), aThreadName);
}