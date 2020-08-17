#pragma once
#include "System/System.h"

#include "Utilities/Singleton.h"

#include <mutex>
#include <type_traits>
#include <unordered_map>

class SystemContainer : public Singleton<SystemContainer>
{
public:

	template<typename TSystem>
	TSystem& GetSystem() requires std::is_base_of_v<System, TSystem>
	{
		std::lock_guard<decltype(myMutex)> lock(myMutex);

		static struct SystemConstructionWrapper
		{
			SystemConstructionWrapper(std::vector<System*>& aSystems)
			{
				aSystems.push_back(&mySystem);
			}

			TSystem mySystem;
		} SystemWrapped(mySystems);

		return SystemWrapped.mySystem;
	}

	template<typename TSystem>
	void CreateSystem()
	{
		GetSystem<TSystem>();
	}

private:
	std::vector<System*> mySystems;
	std::recursive_mutex myMutex;
};

