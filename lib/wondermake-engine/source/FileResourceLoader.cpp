#include "FileResourceLoader.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-io/FileResourceSystem.h"
#include "wondermake-io/ReadFileJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WmLogTags.h"

#include "json/json.hpp"

WM_REGISTER_SYSTEM(FileResourceLoader);

void FileResourceLoader::Initialize()
{
	auto& configSys = Get<ConfigurationSystem>();

	FilePath resourceLoaderFile = configSys.Get<FilePath>(ConfigurationEngine::ResourceLoaderFile, FilePath());

	if (resourceLoaderFile.Location == FilePath::EFolder::Unset && resourceLoaderFile.Path.empty())
	{
		WmLogInfo(TagWonderMake << TagWmResources << "No resource loader file specified, skipping.");

		return;
	}

	WmLogInfo(TagWonderMake << TagWmResources << "Reading resource loader file...");

	auto& jobSys = Get<JobSystem>();
	
	jobSys.StartJob<ReadFileJob>(resourceLoaderFile)
		.ThenRun(GetExecutor(), FutureRunResult([this, resourceLoaderFile](auto aResult)
			{
				WmLogInfo(TagWonderMake << TagWmResources << "Parsing resource loader file...");

				if (!aResult)
				{
					WmLogError(TagWonderMake << TagWmResources << "Failed to load resource loader file; error: " << magic_enum::enum_name(aResult.Err()) << ", file: " << resourceLoaderFile << '.');

					return;
				}

				std::vector<u8> jsonBlob = std::move(aResult).Unwrap();

				json j;

				try
				{
					j = json::parse(std::string(jsonBlob.begin(), jsonBlob.end()));
				}
				catch ([[maybe_unused]] json::exception& e)
				{
					WmLogError(TagWonderMake << TagWmResources << "Failed to parse resource loader file to json; error: " << e.what() << ", file: " << resourceLoaderFile << '.');

					return;
				}
				
				if (!j.is_object())
				{
					WmLogError(TagWonderMake << TagWmResources << "Failed to load resource loader file, file must be json object; file: " << resourceLoaderFile << '.');

					return;
				}

				auto it = j.find("keep_loaded");

				if (it == j.end())
				{
					WmLogError(TagWonderMake << TagWmResources << "Failed to load resource loader file, keep_loaded not found; file: " << resourceLoaderFile << '.');

					return;
				}

				if (!it->is_array())
				{
					WmLogError(TagWonderMake << TagWmResources << "Failed to load resource loader file, keep_loaded not an array; file: " << resourceLoaderFile << '.');

					return;
				}

				auto& resSys = Get<FileResourceSystem>();

				for (const auto& [i, resource] : it.value().items())
				{
					if (!resource.is_object())
					{
						WmLogError(TagWonderMake << TagWmResources << "Failed to parse resource loader element, element is not an object; file: " << resourceLoaderFile << ", index: " << i << '.');

						continue;
					}

					auto itType = resource.find("type");
					auto itPath = resource.find("path");

					if (itType == resource.end()
						|| itPath == resource.end()
						|| !itType->is_string()
						|| !itPath->is_object())
					{
						WmLogError(TagWonderMake << TagWmResources << "Failed to parse resource loader element, object value(s) is/are malformed; file: " << resourceLoaderFile << ", index: " << i << '.');

						continue;
					}

					std::string type = *itType;
					std::string folder = *itPath->find("folder");
					std::string location = *itPath->find("location");

					const auto parseFolder = [](const std::string& aFolder)
					{
						if (aFolder == "bin")
							return FilePath::EFolder::Bin;
						if (aFolder == "data")
							return FilePath::EFolder::Data;
						if (aFolder == "user")
							return FilePath::EFolder::User;
						if (aFolder == "userdata")
							return FilePath::EFolder::UserData;

						return FilePath::EFolder::Unset;
					};

					auto path = FilePath(parseFolder(folder), std::move(location));

					resSys.KeepLoaded(type, path);
				}

				WmLogSuccess(TagWonderMake << TagWmResources << "Resource loader file loaded.");
			}))
		.Detach();
}
