#pragma once

#include "Job/Job.h"

#include <filesystem>
#include <functional>
#include <memory>

template<typename TResource>
class CreateResource
	: public Job<>
{
public:
	using Callback = std::function<void()>;

	inline CreateResource(const std::filesystem::path& aFilePath, Callback&& aCallback)
	{
		myCallback = std::move(aCallback);
		myFilePath = aFilePath;
		myResource = std::make_shared<TResource>(myFilePath);
		CompleteSuccess();
	}

	void OnCompleted(EJobResult /*aResult*/) override
	{
		myCallback();
	}

	std::shared_ptr<TResource> myResource;
protected:
	Callback myCallback;
	std::filesystem::path myFilePath;
};
