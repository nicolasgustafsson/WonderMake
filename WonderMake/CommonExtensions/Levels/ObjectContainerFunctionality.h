#pragma once
#include "Functionalities/Functionality.h"
#include "SubobjectFunctionality.h"

//[Nicos]: This kind of Level-Leveldenizen structure can probably be generalized into a more generic architecture of using some kind of object container functionality in the future.
//[Nicos]: The basic idea is just to be able to access the level to spawn and remove objects whenever

struct SObjectContainerComponent : public SComponent
{
	struct SDenizen
	{
		SDenizen(BaseObject&& aDenizen, SubobjectFunctionality& aDenizenFunctionality)
			: DenizenObject(std::move(aDenizen)), DenizenFunctionality(aDenizenFunctionality)
		{

		}
		Object DenizenObject;
		SubobjectFunctionality& DenizenFunctionality;
	};

	plf::colony<SDenizen> Denizens;
};

class ObjectContainerFunctionality : public Functionality<
	Policy::Set<
		PAdd<FunctionalitySystemDelegate<SubobjectFunctionality>, PWrite>,
		PAdd<SObjectContainerComponent, PWrite>>>
{
public:
	~ObjectContainerFunctionality() override;
	void Tick();

	Object& AddDenizen();
	Object& AddDenizen(Object&& aObject);

	void RemoveDenizen(Object& aObject);

	void AddDenizens(plf::colony<Object>&& aObjects);

	void TransferToOtherContainer(ObjectContainerFunctionality& aNewLevel);
};
