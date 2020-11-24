#pragma once
#include <filesystem>
#include "ResourceSystem.h"
#include "Utilities/Debugging/Debugged.h"
#include <Imgui/FileSelector.h>
#include "Asset.h"
#include <json/json.hpp>
#include "Utilities/Container/Container.h"

template<typename TAssetType>
class AssetDatabase 
	: public System<
		Policy::Set<
			PAdd<ResourceSystem<TAssetType>, PWrite>>>
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
		std::ifstream file("AssetDatabases\\" + GetAssetTypeName() + ".json", std::fstream::app);

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

		SweepAssetDirectories();
	}

	~AssetDatabase()
	{
		Save(); 
	}

	void Save()
	{
		nlohmann::json serialized = Serialize();

		std::ofstream file("AssetDatabases\\" + GetAssetTypeName() + ".json");

		file << std::setw(4) << serialized.dump();
	}

	nlohmann::json Serialize()
	{
		nlohmann::json json;

		json["rootPath"] = myRootPath;

		return json;
	}

	void SweepAssetDirectories()
	{
		for (auto&& file : std::filesystem::recursive_directory_iterator(myRootPath))
			myAssets.AddUnique(file.path());

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
		
		static char buffer[255] = "";

		if (strcmp(buffer, "") == 0)
		{
			strcpy_s(buffer, myRootPath.string().c_str());
		}

		if (ImGui::InputText("Root folder", buffer, 255))
		{
			myRootPath = std::filesystem::current_path() / buffer;
		}

		ImGui::Separator();

		for (auto&& asset : myAssets)
		{
			asset.Inspect();
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
			std::string assetTypeName = typeid(TAssetType).name();

			if (assetTypeName.starts_with("class "))
				assetTypeName.erase(assetTypeName.begin(), assetTypeName.begin() + 6);

			if (assetTypeName.starts_with("struct "))
				assetTypeName.erase(assetTypeName.begin(), assetTypeName.begin() + 7);

			return assetTypeName;
		}
	}
	
	Container<Asset<TAssetType>, Iterable> myAssets;
	
	std::filesystem::path myRootPath = std::filesystem::current_path();
};

#define REGISTER_ASSET_DATABASE(aAsset) _REGISTER_SYSTEM_IMPL(AssetDatabase<aAsset>, aAsset##Asset)
