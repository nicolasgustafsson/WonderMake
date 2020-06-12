#include "pch.h"

#include "LevelDesigner.h"

#include "Collision/CollisionFunctionality.h"
#include "Collision/CollisionSystem.h"
#include "Enemy/EnemyControllerFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Levels/LevelPortalFunctionality.h"
#include "Movement/SpinnerFunctionality.h"
#include "Randomizer/Randomizer.h"
#include "Physics/StaticGeometryFunctionality.h"
#include "Designers/BuffDesigner/BuffDesigner.h"
#include "Levels/BuffGiver/BuffGiverFunctionality.h"
#include "Levels/LevelFunctionality.h"

void LevelDesigner::DesignLevel(LevelFunctionality& aLevel)
{
	SLevelGeometry geometry = DesignGeometry();

	myCurrentLevel = &aLevel;
	auto walls = CreateWalls(geometry);
	myCurrentLevel->AddDenizens(std::move(walls));

	auto objects = InstantiateSpaces(geometry);
	myCurrentLevel->AddDenizens(std::move(objects));
}

plf::colony<Object> LevelDesigner::DesignEnemies(const SSpace& aSpace)
{
	SystemPtr<Randomizer> randomizer;
	plf::colony<Object> enemies;

	const auto enemyCount = randomizer->GetRandomNumber<size_t>(2, 5);

	for (size_t i = 0; i < enemyCount; ++i)
	{
		const SVector2f position{ randomizer->GetRandomNumber<f32>(aSpace.TopLeft.X, aSpace.BottomRight.X), randomizer->GetRandomNumber<f32>(aSpace.BottomRight.Y, aSpace.TopLeft.Y) };

		CreateEnemy(position);
	}

	return enemies;
}

void LevelDesigner::DesignPortal(const SSpace& aSpace)
{
	SystemPtr<Randomizer> randomizer;
	Object& portal = myCurrentLevel->AddDenizen();

	portal.Add<LevelPortalFunctionality>();
	portal.Add<SpinnerFunctionality>();
	auto& transform = portal.Add<TransformFunctionality>();
	auto& sprite = portal.Add<SpriteRenderingFunctionality>();

	const SVector2f position = ((aSpace.BottomRight - aSpace.TopLeft) / 2.f) + aSpace.TopLeft;

	transform.SetPosition(position);
	sprite.SetTexture(std::filesystem::current_path() / "Textures/portal.png");
}

void LevelDesigner::DesignBuffTotems(const SSpace& aSpace)
{
	SystemPtr<Randomizer> randomizer;
	const auto totemCount = randomizer->GetRandomNumber<size_t>(1, 1);

	for (size_t i = 0; i < totemCount; ++i)
		DesignBuffTotem(aSpace);
}

void LevelDesigner::DesignBuffTotem(const SSpace& aSpace)
{
	SystemPtr<Randomizer> randomizer;
	Object& totem = myCurrentLevel->AddDenizen();

	auto& buffGiver = totem.Add<BuffGiverFunctionality>();

	auto& transform = totem.Add<TransformFunctionality>();
	auto& sprite = totem.Add<SpriteRenderingFunctionality>();

	const f32 scale = randomizer->GetRandomNumber(25.f, 150.f) / 100.f;

	//[Nicos]: Should be able to let buff designer handle this and inspect buff afterwards to see if it is a buff or debuff
	const EBuffType buffType = randomizer->GetRandomBool() ? EBuffType::Debuff : EBuffType::Buff;

	SBuffRequirements requirements;
	requirements.Type = buffType;
	requirements.Intensity = 1.f;
	buffGiver.Initialize(SystemPtr<BuffDesigner>()->DesignBuff(requirements), scale * 100.f);

	const SVector2f position{ randomizer->GetRandomNumber<f32>(aSpace.TopLeft.X, aSpace.BottomRight.X), randomizer->GetRandomNumber<f32>(aSpace.BottomRight.Y, aSpace.TopLeft.Y) };

	transform.SetPosition(position);
	sprite.SetTexture(std::filesystem::current_path() / (buffType == EBuffType::Buff ? "Textures/totemGreen.png" : "Textures/totemRed.png"));
	sprite.SetScale({ scale, scale });
}

void LevelDesigner::DesignStartPoint(const SSpace& aSpace) 
{
	const SVector2f position = ((aSpace.BottomRight - aSpace.TopLeft) / 2.f) + aSpace.TopLeft;

	myCurrentLevel->SetStartPosition(position);
}

void LevelDesigner::CreateEnemy(const SVector2f aPosition)
{
	Object& enemy = myCurrentLevel->AddDenizen();

	enemy.Add<EnemyControllerFunctionality>();
	auto& transform = enemy.Add<TransformFunctionality>();
    transform.SetPosition(aPosition);
}

plf::colony<Object> LevelDesigner::CreateWalls(SLevelGeometry& aGeometry) const
{
	plf::colony<Object> walls;

	auto firstPoint = aGeometry.MainGeometry.FirstPoint();

	auto side = firstPoint.GetNextSide();

	do
	{
		Object& object = *walls.emplace();

		auto& geometryFunctionality = object.Add<StaticGeometryFunctionality>();

		geometryFunctionality.SetLine(*side.GetStart(), *side.GetEnd());
		geometryFunctionality.SetLine(*side.GetStart(), *side.GetEnd());
		
		++side;
	} while (side.GetStart() != firstPoint);

	return walls;
}

SLevelGeometry LevelDesigner::DesignGeometry() const
{
	SystemPtr<Randomizer> randomizer;
	SLevelGeometry geometry;

	auto side = DesignStartRoom(geometry);
	side = DesignChallengeRoom(side, geometry);
	DesignPortalRoom(side, geometry);

	return geometry;
}

Geometry::PolygonSideOperator LevelDesigner::DesignStartRoom(SLevelGeometry& aGeometry) const
{
	auto side =
		aGeometry.MainGeometry.AppendPoint({ -300.f, -100.f })	//first point
		.AppendPoint({ 0.f, 200.f })				//second point
		.GetNextSide()								//get side between points
		.Extrude(400.f);							//extrude from side to create starting rectangle

	aGeometry.Spaces.insert({ELevelStage::Start, *side.GetPreviousSide().GetStart(),  *side.GetEnd()});

	return side;
}

Geometry::PolygonSideOperator LevelDesigner::DesignChallengeRoom(Geometry::PolygonSideOperator aWallOfExistingRoom, SLevelGeometry& aGeometry) const
{ 
	aWallOfExistingRoom = aWallOfExistingRoom.GetMiddle()		//get middle of newly extruded side
		.SplitIntoSide(50.f)									//make 2 new points with a width of 50.f to become entrance
		.Extrude(30.f)											//extrude to make passage into new room
		.Extrude(0.f)											//creates a second set of points that will act as corners for new room
		.Extrude(300 * 2.f);									//extrude into a new room
	
	aWallOfExistingRoom.GetPreviousSide().Move({ 0.f, 300.f });	//widen room upwards
	aWallOfExistingRoom.GetNextSide().Move({ 0.f, -300.f });	//widen room downwards

	aGeometry.Spaces.insert({ ELevelStage::Challenge, *aWallOfExistingRoom.GetPreviousSide().GetStart(),  *aWallOfExistingRoom.GetEnd() });

	return aWallOfExistingRoom;
}

void LevelDesigner::DesignPortalRoom(Geometry::PolygonSideOperator aWallOfExistingRoom, SLevelGeometry& aGeometry) const
{
	aWallOfExistingRoom = aWallOfExistingRoom.GetMiddle()		//get middle of newly extruded side
		.SplitIntoSide(50.f)									//make 2 new points with a width of 50.f to become entrance
		.Extrude(30.f)											//extrude to make passage into new room
		.Extrude(0.f)											//creates a second set of points that will act as corners for new room
		.Extrude(200.f);										//extrude into a new room

	aWallOfExistingRoom.GetPreviousSide().Move({ 0.f, 75.f });	//widen room upwards
	aWallOfExistingRoom.GetNextSide().Move({ 0.f, -75.f });		//widen room downwards

	aGeometry.Spaces.insert({ ELevelStage::Portal, *aWallOfExistingRoom.GetPreviousSide().GetStart(),  *aWallOfExistingRoom.GetEnd() });
}

plf::colony<Object> LevelDesigner::InstantiateSpaces(const SLevelGeometry& aGeometry)
{
	plf::colony<Object> objects;

	for (auto&& space : aGeometry.Spaces)
	{
		switch (space.SpaceType)
		{
		case ELevelStage::Start:
			DesignStartPoint(space);
			break;

		case ELevelStage::Challenge:
		{
			auto enemies = DesignEnemies(space);
			objects.splice(enemies);
			DesignBuffTotems(space);
			break;
		}

		case ELevelStage::Portal:
			DesignPortal(space);
			break;
		}
	}

	return objects; 
}
