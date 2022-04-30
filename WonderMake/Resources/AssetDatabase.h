#pragma once

#include <filesystem>
#include <json/json.hpp>
#include <Imgui/imgui.h>
#include <Imgui/imgui_stdlib.h>

#include "Asset.h"
#include "Graphics/Shader.h"
#include "ResourceSystem.h"

#include "Utilities/Debugging/Debugged.h"
#include "Utilities/Container/Container.h"
#include "Utilities/Id.h"

#include "Imgui/AssetBrowser.h"
#include "Imgui/FileSelector.h"

#include "AssetLink.h"

template<typename TAssetType>
class AssetDatabase 
	: public System<ResourceSystem<TAssetType>>
	, public Debugged
{
public:
	AssetDatabase() 
		: Debugged("Asset Databases/" + GetAssetTypeName()) 
	{
		Load();
	}

	void Load()
	{
		std::ifstream file("AssetDatabases/" + GetAssetTypeName() + ".json", std::fstream::app);

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

		for (auto& assetLink : myAssetLinks)
		{
			LocateAsset(assetLink.second);
		}

		Save();
	}

	void LocateAsset(SAssetLink<TAssetType>& aAssetLink)
	{
		if (!aAssetLink.AssetId || !aAssetLink.AssetPath)
			return;

		if (!myAssets.KeyExists(*(aAssetLink.AssetId)))
			return;

		if (myAssets.Get(*(aAssetLink.AssetId)).myMetadata.Filepath != aAssetLink.AssetPath)
		{
			for(auto& it : myAssets)
			{
				Asset<TAssetType>& asset = it.second;
				if (asset.myMetadata.Filepath == aAssetLink.AssetPath)
				{
					aAssetLink.AssetId = asset.myMetadata.AssetId;
					aAssetLink.SavedAssetId = asset.myMetadata.AssetId;
					WmLog(TagSuccess, "Located asset with path ", *(aAssetLink.AssetPath), "!");

					return;
				}
			}

			WmLog(TagError, "Could not locate asset with path: ", *(aAssetLink.AssetPath));
			aAssetLink.AssetId.reset();
			aAssetLink.SavedAssetId.reset();
		}
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

		std::ofstream file("AssetDatabases/" + GetAssetTypeName() + ".json");

		file << serialized.dump(4);
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

		for (auto&& file : std::filesystem::recursive_directory_iterator(std::filesystem::current_path() / myRootPath))
		{
			Id assetId = myIdCounter.NextId();
			auto relativePath = std::filesystem::proximate(file.path(), std::filesystem::current_path());
			Asset<TAssetType> asset{ relativePath, assetId };
			asset.LoadAsset();
			myAssets.Add(assetId, asset);
		}

		Save();
	}

	virtual void Debug() override
	{
		std::string assetDatabaseName = GetAssetTypeName() + " Database";
		ImGui::Begin(assetDatabaseName.c_str());
		
		if (ImGui::Button("Scan assets"))
		{
			SweepAssetDirectories();
			WmLog(TagSuccess, assetDatabaseName + " scanned assets!");
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
	constexpr std::string GetAssetTypeName() const
	{
		//[Nicos]: Special case shaders cause otherwise they show up as Shader<0> etc
		if constexpr (std::is_same_v<TAssetType, Shader<EShaderType::Fragment>>)
			return "Fragment Shader";
		else if constexpr (std::is_same_v<TAssetType, Shader<EShaderType::Vertex>>)
			return "Vertex Shader";
		else if constexpr (std::is_same_v<TAssetType, Shader<EShaderType::Geometry>>)
			return "Geometry Shader";
		else
		{
            return std::string(TypeString<TAssetType>());
            //std::string assetTypeName = typeid(TAssetType).name();

			//if (assetTypeName.starts_with("class "))
			//	assetTypeName.erase(assetTypeName.begin(), assetTypeName.begin() + 6);

			//if (assetTypeName.starts_with("struct "))
			//	assetTypeName.erase(assetTypeName.begin(), assetTypeName.begin() + 7);

			//return assetTypeName;
		}
	}
	
	Container<Asset<TAssetType>, Key<Id>, Iterable, StableElements> myAssets;

	Container<SAssetLink<TAssetType>, Key<std::string>, Iterable> myAssetLinks;
	
	std::filesystem::path myRootPath = {};

	IdCounter myIdCounter;
};

#define REGISTER_ASSET_DATABASE(aAsset) _REGISTER_SYSTEM_IMPL(AssetDatabase<aAsset>, aAsset##Asset)
