#pragma once

#include "FileSystem/ReadFileAsStringJob.h"

#include "System/System.h"

#include <filesystem>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>

class FileSystem
	: public System
{
public:
	template<typename TCallback>
	inline void ReadFileAsString(std::filesystem::path aFilePath, TCallback aCallback);

private:
	std::vector<std::shared_ptr<Job>> myFileJobs;
	std::mutex myLock;
};

template<typename TCallback>
inline void FileSystem::ReadFileAsString(std::filesystem::path aFilePath, TCallback aCallback)
{
	auto job = std::make_shared<ReadFileAsStringJob>();
	job->Setup(std::move(aFilePath), [&, wjob = std::weak_ptr<ReadFileAsStringJob>(job), aCallback = std::move(aCallback)](const Job::EResult aResult)
	{
		auto job = wjob.lock();
		if (!job)
		{
			aCallback(aResult, std::string());
			return;
		}
		{
			std::lock_guard<decltype(myLock)> lock(myLock);
			std::remove(myFileJobs.begin(), myFileJobs.end(), job);
		}
		aCallback(aResult, std::move(job->myOutData));
	});
	job->Start();
	std::lock_guard<decltype(myLock)> lock(myLock);
	myFileJobs.emplace_back(std::move(job));
}