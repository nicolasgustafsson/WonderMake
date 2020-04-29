#pragma once
#include <json/json.hpp>


namespace ImGui::JsonInspector
{
	void Inspect(nlohmann::json& aJsonDocumentToInspect, const std::string aDocumentName);

	void InspectElement(json::iterator aElement);

	void InspectFloat(nlohmann::json::iterator aElementIterator);

	void InspectObject(nlohmann::json::iterator aElementIterator);
	void InspectInteger(nlohmann::json::iterator aElementIterator);
	void InspectString(nlohmann::json::iterator aElementIterator);
	void InspectTheUninspectable(nlohmann::json::iterator aElementIterator);
	void InspectBool(nlohmann::json::iterator aElementIterator);
};

