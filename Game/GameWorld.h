#pragma once
#include <System/System.h>


#include <Object/ComponentSystem.h>
#include <System/SystemPtr.h>
#include <Object/Object.h>
#include <Object/ComponentDependency.h>
#include <Object/Component.h>
#include <Object/Functionality.h>
#include <Object/FunctionalityDependency.h>
#include <Object/Components/TransformComponent.h>
#include <Object/Functionalities/SpriteRenderingFunctionality.h>

class MovementFunctionality
	: public Functionality
{
public:
	MovementFunctionality(Object* OwningObject)
		:myPositionComponent(OwningObject)
	{

	}

	virtual void Tick() override
	{
		int bla = rand() % 4;
		int amplitude = rand() % 20;

		if (bla == 0)
			myPositionComponent->Position.X += amplitude;
		else if (bla == 1)
			myPositionComponent->Position.X -= amplitude;
		else if (bla == 2)
			myPositionComponent->Position.Y += amplitude;
		else if (bla == 3)
			myPositionComponent->Position.Y -= amplitude;
	}
	ComponentDependency<STransformComponent> myPositionComponent;
};

class Player
	: public Object
{
public:
	Player()
		:myMovement(this)
		, mySprite(this)
	{
		mySprite->SetTexture(std::filesystem::current_path() / "Textures/tile.png");
	}

private:
	FunctionalityDependency<MovementFunctionality> myMovement;
	FunctionalityDependency<SpriteRenderingFunctionality> mySprite;
};

class Ant
	:public Object 
{
public:
	Ant()
		:myMovement(this)
		, mySprite(this)
	{
		mySprite->SetTexture(std::filesystem::current_path() / "Textures/tile.png");
		mySprite->SetScale({ 0.03f, 0.03f });
	}

private:
	FunctionalityDependency<MovementFunctionality> myMovement;
	FunctionalityDependency<SpriteRenderingFunctionality> mySprite;
};

class GameWorld : public System
{
public:
	GameWorld();
	~GameWorld();

	virtual void Tick() override;
	//Player myPlayer;

	std::array<Ant, 10> myAnts;
};

