#pragma once
#include "Levels/Level.h"

#include "System/System.h"

struct SRoom
{
	struct SWall
	{
		SVector2f Start;
		SVector2f End;
	};

	plf::colony<SWall> Walls;
};

class LevelDesigner
	: public System
{
public:
	Level DesignLevel() const;

protected:
	plf::colony<Object> DesignGeometry() const;
	SRoom DesignRoom() const;

	plf::colony<Object> DesignEnemies() const;
	Object DesignPortal() const;

};
