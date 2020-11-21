#pragma once

#include "Job/Job.h"

#include <filesystem>
#include <memory>

class ReadFileAsStringJob
	: public Job
	, public std::enable_shared_from_this<ReadFileAsStringJob>
{
public:
	template<typename TCallback>
	inline void Setup(std::filesystem::path&& aFilePath, TCallback&& aCallback);

	void Start();

	std::string myOutData;

private:
	std::filesystem::path myFilePath;

	void ReadFile();
};

template<typename TCallback>
inline void ReadFileAsStringJob::Setup(std::filesystem::path&& aFilePath, TCallback&& aCallback)
{
	myFilePath = aFilePath;
	myCallback = aCallback;
}
