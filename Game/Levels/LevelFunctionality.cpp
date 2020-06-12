#include "pch.h"
#include "LevelFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"

LevelFunctionality::LevelFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

Object& LevelFunctionality::AddDenizen(Object&& aObject)
{
	return Get<SLevelComponent>().Denizens.emplace(std::move(aObject), aObject.Add<LevelDenizenFunctionality>())->DenizenObject;
}

Object& LevelFunctionality::AddDenizen()
{
	return AddDenizen({});
}

void LevelFunctionality::AddDenizens(plf::colony<Object>&& aObjects)
{
	for (auto&& object : aObjects)
		AddDenizen(std::move(object));
}

void LevelFunctionality::SetStartPosition(const SVector2f aPosition)
{
	Get<SLevelComponent>().StartPosition = aPosition;
}
