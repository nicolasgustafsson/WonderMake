#include "pch.h"

#include "Utilities/Platform.h"

#include "wondermake-utility/WinPlatform.h"

#include <processthreadsapi.h>
#include <string>
#include <time.h>

namespace Platform
{
	const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)


	void SetThreadName(const DWORD aThreadId, const std::string& threadName)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName.c_str();
		info.dwThreadID = aThreadId;
		info.dwFlags = 0;

		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	void SetThreadName(const std::string& threadName)
	{
		SetThreadName(GetCurrentThreadId(), threadName);
	}

	void SetThreadName(std::thread& aThread, const std::string& threadName)
	{
		DWORD threadId = ::GetThreadId(static_cast<HANDLE>(aThread.native_handle()));
		SetThreadName(threadId, threadName);
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
}
