#pragma once
#include "Functionalities/TransformFunctionality.h"
#include "Geometry/Polygon.h"
#include "System/System.h" 

class ObjectContainerFunctionality;
class LevelPortalFunctionality;
class SpinnerFunctionality;
class SpriteRenderingFunctionality;
class BuffGiverFunctionality;
class EnemyControllerFunctionality;
class CollisionFunctionality;
class StaticGeometryFunctionality;
class DebugSettingsSystem;

class BuffDesigner;
class Randomizer;

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
	: public System<
		Policy::Set<
			PAdd<Randomizer, PWrite>,
			PAdd<BuffDesigner, PWrite>,
			PAdd<FunctionalitySystemDelegate<LevelPortalFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<SpinnerFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<TransformFunctionality2D>, PWrite>,
			//[Nicos]: TODO REMOVE - After we have chains and stuff
			PAdd<FunctionalitySystemDelegate<CollisionFunctionality>, PWrite>,
			PAdd<DebugSettingsSystem, PWrite>,
			PAdd<FunctionalitySystemDelegate<SpriteRenderingFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<BuffGiverFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<EnemyControllerFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<StaticGeometryFunctionality>, PWrite>>>
{
public:
	using Super::Super;

	void DesignLevel(ObjectContainerFunctionality& aLevel);

protected:
	plf::colony<Object> CreateWalls(SLevelGeometry& aGeometry);
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

	ObjectContainerFunctionality* myCurrentLevel;

	SLevelGeometry myGeometry;
};
