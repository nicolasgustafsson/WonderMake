#pragma once

#include "Job/Job.h"
#include "Threads/RoutineIds.h"

#include <filesystem>
#include <memory>

class ReadFileAsString
	: public Job
	, public std::enable_shared_from_this<ReadFileAsString>
{
public:
	template<typename TCallback>
	inline void Setup(std::filesystem::path&& aFilePath, TCallback&& aCallback);

	void Start();

	std::string myOutData;

private:
	std::filesystem::path myFilePath;
	RoutineChecker myRoutineChecker;

	void ReadFile();
};

template<typename TCallback>
inline void ReadFileAsString::Setup(std::filesystem::path&& aFilePath, TCallback&& aCallback)
{
	myFilePath = aFilePath;
	myCallback = aCallback;
}
