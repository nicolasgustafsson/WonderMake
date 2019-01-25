#pragma once
#include "Utilities/plf_colony.h"
#include "System/System.h"

template<typename TFunctionality>
class FunctionalitySystem
	:public System
{
public:
	FunctionalitySystem();
	TFunctionality* AddFunctionality(Object* aObject);
	void RemoveFunctionality(TFunctionality* const aFunctionality);

	bool IsEmpty() const;

	virtual void Tick() override;

private:
	plf::colony<TFunctionality> myFunctionalities;
};

template<typename TFunctionality>
void FunctionalitySystem<TFunctionality>::Tick()
{
	for (auto&& capability : myFunctionalities)
		capability.Tick();
}

template<typename TFunctionality>
FunctionalitySystem<TFunctionality>::FunctionalitySystem()
{
	if (typeid(&TFunctionality::Tick) != typeid(&BaseFunctionality::Tick))
		EnableTick();
}

template<typename TFunctionality>
bool FunctionalitySystem<TFunctionality>::IsEmpty() const
{
	return myFunctionalities.size() == 0;
}

template<typename TFunctionality>
void FunctionalitySystem<TFunctionality>::RemoveFunctionality(TFunctionality* const aFunctionality)
{
	myFunctionalities.erase(myFunctionalities.get_iterator_from_pointer(aFunctionality));
}

template<typename TFunctionality>
typename TFunctionality* FunctionalitySystem<TFunctionality>::AddFunctionality(Object* aObject)
{
	return &(*myFunctionalities.emplace(aObject));
}

