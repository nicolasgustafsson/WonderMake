#pragma once

#include "Job/Job.h"

#include <filesystem>
#include <memory>

template<typename TResource>
class CreateResource
	: public Job
{
public:
	inline void Setup(const std::filesystem::path& aFilePath, Callback&& aCallback)
	{
		SetCallback(std::move(aCallback));
		myFilePath = aFilePath;
	}

	inline virtual void OnStarted() override
	{
		myResource = std::make_shared<TResource>(myFilePath);
		CompleteSuccess();
	}

	std::shared_ptr<TResource> myResource;
protected:
	std::filesystem::path myFilePath;
};
