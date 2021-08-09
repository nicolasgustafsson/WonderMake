#pragma once
#include "Utilities/Id.h"
#include "Resources/AssetLink.h"

namespace WmGui
{
	template<std::input_iterator TIterator>
	[[nodiscard]] std::optional<SAssetMetadata> InlineAssetBrowser(TIterator aBegin, TIterator aEnd, std::optional<Id> aCurrentSelected)
	{
		using AssetType = typename TIterator::value_type;

		std::optional<SAssetMetadata> selectedAsset;

		static ImGuiTableFlags flags =
			ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
			| ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
			| ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody;

		if (ImGui::BeginTable("##table", 1, flags))
		{
			// Declare columns
			ImGui::TableSetupColumn("Filepath");
			ImGui::TableSetupScrollFreeze(1,1);

			ImGui::TableHeadersRow();

			//for (int row_n = 0; row_n < items.Size; row_n++)
			for (auto it = aBegin; it != aEnd; it++)
			{
				AssetType& asset = *it;

				const bool itemIsSelected = asset.second.myMetadata.AssetId == aCurrentSelected;

				if (itemIsSelected)
					selectedAsset = asset.second.myMetadata;

				ImGui::PushID(static_cast<i32>(asset.second.myMetadata.AssetId.GetRawId()));
				ImGui::TableNextRow(ImGuiTableRowFlags_None);
				ImGui::TableNextColumn();

				std::string idLabel = "";
				idLabel += std::to_string(asset.second.myMetadata.AssetId.GetRawId());

				if (ImGui::Selectable(asset.second.myMetadata.Filepath.string().c_str(), itemIsSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, 0)))
					selectedAsset = asset.second.myMetadata;

				ImGui::PopID();
			}
		}

		ImGui::EndTable();

		return selectedAsset;
	}

	//returns true if a new asset was picked
	template<std::input_iterator TIterator, class TAssetType>
	[[nodiscard]] bool AssetPicker(TIterator aBegin, TIterator aEnd, std::string_view aAssetLinkName, SAssetLink<TAssetType>& aAssetLink)
	{
		std::string windowName = "Pick an asset for <" + std::string(aAssetLinkName) + ">";
		ImGui::Begin(windowName.c_str());

		std::optional<SAssetMetadata> assetMetadata = WmGui::InlineAssetBrowser(aBegin, aEnd, aAssetLink.AssetId);

		if (assetMetadata)
			aAssetLink.AssetId = assetMetadata->AssetId;
		else
			aAssetLink.AssetId.reset();

		bool pickedNewAsset = false;

		if (ImGui::Button("Pick Asset"))
		{
			if (assetMetadata)
			{
				aAssetLink.SavedAssetId = assetMetadata->AssetId;
				aAssetLink.AssetPath = assetMetadata->Filepath;
			}
			else
			{
				aAssetLink.SavedAssetId.reset();
				aAssetLink.AssetPath.reset();
			}
			aAssetLink.IsSelectingAsset = false;
			pickedNewAsset = true;
		}
		
		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			aAssetLink.AssetId = aAssetLink.SavedAssetId;
			aAssetLink.IsSelectingAsset = false;
		}

		ImGui::End();

		return pickedNewAsset;
	}
};

