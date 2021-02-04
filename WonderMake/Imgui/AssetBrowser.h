#pragma once
#include "Utilities/Id.h"
#include "Resources/AssetLink.h"

namespace WmGui
{
	template<std::input_iterator TIterator>
	[[nodiscard]] std::optional<Id> InlineAssetBrowser(TIterator aBegin, TIterator aEnd, std::optional<Id> aCurrentSelected)
	{
		using AssetType = typename TIterator::value_type;

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

				const bool item_is_selected = asset.second.myMetadata.AssetId == aCurrentSelected;
				ImGui::PushID(static_cast<i32>(asset.second.myMetadata.AssetId.GetRawId()));
				ImGui::TableNextRow(ImGuiTableRowFlags_None);
				ImGui::TableNextColumn();

				std::string idLabel = "";
				idLabel += std::to_string(asset.second.myMetadata.AssetId.GetRawId());

				if (ImGui::Selectable(asset.second.myMetadata.Filepath.string().c_str(), item_is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, 0)))
					aCurrentSelected = asset.second.myMetadata.AssetId;

				ImGui::PopID();
			}
		}

		ImGui::EndTable();

		return aCurrentSelected;
	}

	//returns true if a new asset was picked
	template<std::input_iterator TIterator, class TAssetType>
	[[nodiscard]] bool AssetPicker(TIterator aBegin, TIterator aEnd, std::string_view aAssetLinkName, SAssetLink<TAssetType>& aAssetLink)
	{
		std::string windowName = "Pick an asset for <" + std::string(aAssetLinkName) + ">";
		ImGui::Begin(windowName.c_str());

		aAssetLink.AssetId = WmGui::InlineAssetBrowser(aBegin, aEnd, aAssetLink.AssetId);

		bool pickedNewAsset = false;

		if (ImGui::Button("Pick Asset"))
		{
			aAssetLink.SavedAssetId = aAssetLink.AssetId;
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

