#pragma once
#include <type_traits>

#include "System.h"
#include <unordered_map>
#include <Utilities/Singleton.h>

class SystemContainer : public Singleton<SystemContainer>
{
public:

	template<typename TSystem>
	TSystem& GetSystem()
	{
		static_assert(std::is_base_of<System, TSystem>::value, "Tried to get system that does not inherit from System!");

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

private:
	std::vector<System*> mySystems;
};

