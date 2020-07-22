#pragma once

#include "Physics/Navmesh/Navmesh.h"
#include "Physics/Navmesh/NavmeshRenderObject.h"

struct SNavmeshComponent : public SComponent
{
	Navmesh Navmesh;

	std::optional<NavmeshRenderObject> RenderObject;
};

class NavmeshFunctionality : public Functionality<NavmeshFunctionality, 
	Policy::Set<
	Policy::Add<SNavmeshComponent, Policy::EPermission::Write>>>
{
public:
	NavmeshFunctionality(Object& aOwner);

	void SetNavmesh(Navmesh aNavmesh);

	void Tick();
};

