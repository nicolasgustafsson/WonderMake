#pragma once

#include "Physics/Navmesh/Navmesh.h"
#include "Physics/Navmesh/NavmeshRenderObject.h"

struct SNavmeshComponent : public SComponent
{
	Navmesh Navmesh;

	std::optional<NavmeshRenderObject> Background;
	std::optional<NavmeshRenderObject> Walls;
	std::optional<NavmeshRenderObject> Walls2;
	std::optional<NavmeshRenderObject> Walls3;
	std::optional<NavmeshRenderObject> Walls4;
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

