#include "JsonInspector.h"

#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-utility/Typedefs.h"

bool ImGui::JsonInspector::Inspect(nlohmann::json& aJsonDocumentToInspect, const std::string aDocumentName)
{
	bool open = true;

	if (ImGui::Begin(aDocumentName.c_str(), &open))
	{
		if (open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(2);
			ImGui::Separator();

			for (nlohmann::json::iterator element = aJsonDocumentToInspect.begin(); element != aJsonDocumentToInspect.end(); element++)
			{
				InspectElement(element);
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::PopStyleVar();
		}
	}

	ImGui::End();

	return open;
}

void ImGui::JsonInspector::InspectElement(nlohmann::json::iterator aElementIterator)
{
	nlohmann::json& element = *aElementIterator;
	
	if (element.type() == nlohmann::json::value_t::object)
	{
		InspectObject(aElementIterator);
		return;
	}
	
	ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, aElementIterator.key().c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	ImGui::PushID(&element);
	
	switch (element.type())
	{
	case nlohmann::json::value_t::number_float:
		InspectFloat(aElementIterator);
		break;
	case nlohmann::json::value_t::number_integer:
		InspectInteger(aElementIterator);
		break;
	case nlohmann::json::value_t::string:
		InspectString(aElementIterator);
		break;
	case nlohmann::json::value_t::boolean:
		InspectBool(aElementIterator);
		break;
	default:
		InspectTheUninspectable(aElementIterator);
		break;
	}
	
	ImGui::PopID();
	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

void ImGui::JsonInspector::InspectFloat(nlohmann::json::iterator aElementIterator)
{
	nlohmann::json& element = *aElementIterator;

	f64& ref = element.get_ref<f64&>();
	ImGui::InputDouble("##value", &ref, 1.0f);
}

void ImGui::JsonInspector::InspectObject(nlohmann::json::iterator aElementIterator)
{
	nlohmann::json& element = *aElementIterator;

	ImGui::PushID(&element);
	bool node_open = ImGui::TreeNode("Object", aElementIterator.key().c_str());
	ImGui::NextColumn();
	ImGui::NextColumn();

	if (node_open)
	{
		for (nlohmann::json::iterator childElement = element.begin(); childElement != element.end(); childElement++)
		{
			InspectElement(childElement);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void ImGui::JsonInspector::InspectInteger(nlohmann::json::iterator aElementIterator)
{
	nlohmann::json& element = *aElementIterator;

	i64& ref = element.get_ref<i64&>();

	i32 integer = static_cast<i32>(ref);
	ImGui::InputInt("##value", &integer, 1);
	ref = integer;
}

void ImGui::JsonInspector::InspectString(nlohmann::json::iterator aElementIterator)
{
	nlohmann::json& element = *aElementIterator;

	std::string& ref = element.get_ref<std::string&>();

	ImGui::InputText("##value", &ref[0], ref.capacity());
}

void ImGui::JsonInspector::InspectTheUninspectable(nlohmann::json::iterator /*aElementIterator*/)
{
	ImGui::Text("Uninspectable");
}

void ImGui::JsonInspector::InspectBool(nlohmann::json::iterator aElementIterator)
{
	nlohmann::json& element = *aElementIterator;

	bool& ref = element.get_ref<bool&>();

	ImGui::Checkbox("##value", &ref);
}
