#pragma once
#include <optional>

//base class?
//or something else
struct SAttribute
{
	virtual ~SAttribute() = default;
	//Describes idea
};

template<typename TDesignedObject>
struct SDesignedObjectAttribute : public SAttribute
{
	TDesignedObject FinishedDesign;
};

template<typename TAttribute>
struct SGenericAttribute : public SAttribute
{
	TAttribute Attribute;
};