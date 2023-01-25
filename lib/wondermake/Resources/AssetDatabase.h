#pragma once

#include "Imgui/AssetBrowser.h"
#include "Imgui/FileSelector.h"
#include "Resources/Asset.h"
#include "Resources/AssetLink.h"
#include "Resources/ResourceSystem.h"
#include "Utilities/Debugging/Debugged.h"
#include "Utilities/Container/Container.h"
#include "Utilities/Id.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include <Imgui/imgui_stdlib.h>

#include <json/json.hpp>

#include <filesystem>
#include <fstream>

template<typename TAssetType>
class AssetDatabase 
	: public System<
		Policy::Set<
			PAdd<ResourceSystem<TAssetType>, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
	, public Debugged
{
public:
	AssetDatabase() 
		: Debugged("Asset Databases/" + GetResourceTypeName<TAssetType>())
	{
		Load();
	}

	void Load()
	{
		std::ifstream file("AssetDatabases\\" + GetResourceTypeName<TAssetType>() + ".json", std::fstream::app);

		std::string fileContents((std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>()));

		nlohmann::json jsonFile;

		if (json::accept(fileContents))
		{
			jsonFile = json::parse(fileContents);
			Deserialize(jsonFile);
		}
	}

	void Deserialize(const nlohmann::json& aJson)
	{
		myRootPath = aJson["rootPath"].get<std::filesystem::path>();
		myAssetLinks = aJson["container"].get<decltype(myAssetLinks)>();

		SweepAssetDirectories();
	}

	Asset<TAssetType>* GetAsset(std::string_view aAssetLink)
	{
		auto assetId = myAssetLinks[std::string(aAssetLink)].AssetId;

		if (assetId)
		{
			if (!myAssets.KeyExists(*assetId))
				return nullptr;
			
			return &myAssets.Get(*assetId);
		}
		
		return nullptr;
	}

	ResourceProxy<TAssetType> GetResource(std::string_view aAssetLink)
	{
		Asset<TAssetType>* asset = GetAsset(aAssetLink);

		if (!asset)
			return {};

		if (!asset->myResource)
			return {};

		return *(asset->myResource);
	}

	~AssetDatabase()
	{
		Save(); 
	}

	void Save()
	{
		nlohmann::json serialized = Serialize();

		std::ofstream file("AssetDatabases\\" + GetResourceTypeName<TAssetType>() + ".json");

		file << std::setw(4) << serialized.dump();
	}

	nlohmann::json Serialize()
	{
		nlohmann::json json;

		json["rootPath"] = myRootPath;
		json["container"] = myAssetLinks;

		return json;
	}

	void SweepAssetDirectories()
	{
		myAssets.EraseIf([](auto& aAsset)
			{
				return !std::filesystem::exists(aAsset.second.myMetadata.Filepath);
			});

		auto& resourceSys = this->Get<ResourceSystem<TAssetType>>();

		for (auto&& file : std::filesystem::recursive_directory_iterator(std::filesystem::current_path() / myRootPath))
		{
			Id assetId = myIdCounter.NextId();
			auto relativePath = std::filesystem::proximate(file.path(), std::filesystem::current_path());
			Asset<TAssetType> asset{ relativePath, assetId };
			asset.LoadAsset(resourceSys);
			myAssets.Add(assetId, asset);

		}

		Save();
	}

	virtual void Debug() override
	{
		std::string assetDatabaseName = GetResourceTypeName<TAssetType>() + " Database";
		ImGui::Begin(assetDatabaseName.c_str());
		
		if (ImGui::Button("Scan assets"))
		{
			SweepAssetDirectories();
			WmLogSuccess(TagWonderMake << TagWmResources << assetDatabaseName << " scanned assets.");
		}

		static std::string buffer = myRootPath.string();

		if (ImGui::InputText("Root folder", &buffer))
			myRootPath = buffer;

		ImGui::Separator();

		if (ImGui::TreeNode("Assets"))
		{
			for (auto&& asset : myAssets)
			{
				asset.second.Inspect();
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Asset Links"))
		{
			for (auto&& assetLink : myAssetLinks)
			{
				ImGui::PushID(assetLink.first.c_str());

				if (ImGui::Button("Show asset browser"))
					assetLink.second.IsSelectingAsset = true;

				ImGui::SameLine();

				ImGui::Text(assetLink.first.c_str());

				if (assetLink.second.IsSelectingAsset)
					if (WmGui::AssetPicker(myAssets.begin(), myAssets.end(), assetLink.first, assetLink.second))
						Save();

				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		ImGui::End();
	}

private:
	Container<Asset<TAssetType>, Key<Id>, Iterable, StableElements> myAssets;

	Container<SAssetLink<TAssetType>, Key<std::string>, Iterable> myAssetLinks;
	
	std::filesystem::path myRootPath = {};

	IdCounter myIdCounter;
};

#define WM_REGISTER_ASSET_DATABASE(aAsset) _WM_REGISTER_SYSTEM_IMPL(AssetDatabase<aAsset>, aAsset##Asset)
