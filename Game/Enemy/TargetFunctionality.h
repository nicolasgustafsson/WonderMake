#pragma once
#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

class TargetFunctionality
	: public Functionality<TargetFunctionality, TransformFunctionality>
{
public:
	TargetFunctionality(Object& aOwner);

	template<typename TPredicate>
	[[nodiscard]] inline TransformFunctionality* FindTarget(const TPredicate& aPredicate) const noexcept;

	// (Kevin): This is temporary solution. The real solution is to fetch a list of targets and then filter through them using the predicate.
	TransformFunctionality* Temp;
private:
	
};

template<typename TPredicate>
[[nodiscard]] inline TransformFunctionality* TargetFunctionality::FindTarget(const TPredicate& /*aPredicate*/) const noexcept
{
	return Temp;
}
