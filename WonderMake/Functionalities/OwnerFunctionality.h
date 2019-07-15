#pragma once
class OwnerFunctionality
	: public Functionality<OwnerFunctionality>
{
public:
	OwnerFunctionality(Object& aOwner);

	Object& GetOwner() const;

private:
	Object& myOwner;
};

