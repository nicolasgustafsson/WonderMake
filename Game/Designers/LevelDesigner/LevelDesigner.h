#pragma once
#include "Levels/Level.h"

#include "System/System.h"

class LevelDesigner
	: public System
{
public:
	Level DesignLevel();

protected:

	plf::colony<Object> DesignEnemies() const;

};
