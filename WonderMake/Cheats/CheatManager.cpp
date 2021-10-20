#include "pch.h"
#include "CheatManager.h"

REGISTER_SYSTEM(CheatManager);

void CheatManager::AddCheat(std::string aCheatName, Closure&& aCheat)
{
	myCheats.Add(aCheatName, std::move(aCheat));
}

void CheatManager::RemoveCheat(std::string aCheatName)
{
	myCheats.Erase(aCheatName);
}

void CheatManager::Debug()
{
	ImGui::Begin("CheatManager");

	bool odd = true;
	for(auto&& cheat : myCheats)
	{
		if (ImGui::Button(cheat.first.c_str()))
		{
			cheat.second();
		}
		if (odd)
			ImGui::SameLine();

		odd = !odd;
	}
	ImGui::End();
}
