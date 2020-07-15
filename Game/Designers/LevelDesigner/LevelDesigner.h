#pragma once
#include "System/System.h" 
#include "Geometry/Polygon.h"

class LevelFunctionality;

namespace Geometry { class PolygonSideOperator; }

enum class ELevelStage
{
	Start,
	Portal,
	Challenge
};

struct SSpace
{
	ELevelStage SpaceType;

	//Nicos todo: this should be a polygon!
	SVector2f TopLeft;
	SVector2f BottomRight;
};

struct SLevelGeometry
{
	Geometry::Polygon MainGeometry;

	plf::colony<SSpace> Spaces;
};

struct SRoom
{
	SVector2f Width;
	SVector2f Height;

	struct SWallSection
	{
		SVector2f Start;
		SVector2f End;
	};

	plf::colony<SWallSection> Walls;

	plf::colony<SWallSection> Connections;
};

class LevelDesigner
	: public System, public Debugged
{
public:
	LevelDesigner();
	void DesignLevel(LevelFunctionality& aLevel);

protected:
	plf::colony<Object> CreateWalls(SLevelGeometry& aGeometry) const;
	SLevelGeometry DesignGeometry() const;

	Geometry::PolygonSideOperator DesignStartRoom(SLevelGeometry& aGeometry) const;
	Geometry::PolygonSideOperator DesignChallengeRoom(Geometry::PolygonSideOperator aWallOfExistingRoom, SLevelGeometry& aGeometry) const;
	void DesignPortalRoom(Geometry::PolygonSideOperator aWallOfExistingRoom, SLevelGeometry& aGeometry) const;

	plf::colony<Object> InstantiateSpaces(const SLevelGeometry& aGeometry);

	plf::colony<Object> DesignEnemies(const SSpace& aSpace);
	void DesignPortal(const SSpace& aSpace);
	void DesignBuffTotems(const SSpace& aSpace);
	void DesignBuffTotem(const SSpace& aSpace);
	void DesignStartPoint(const SSpace& aSpace);
	void CreateEnemy(const SVector2f aPosition);

	LevelFunctionality* myCurrentLevel;

	SLevelGeometry myGeometry;

	void Debug() override;
};
