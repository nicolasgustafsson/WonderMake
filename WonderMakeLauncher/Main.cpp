#include "pch.h"

#include "Game.h"

#include "Program/Engine.h"

#include "Message/Messages.h"
#include <iostream>

int main()
{
	Engine::Start([&] 
	{
		Start();
		WmLog("Game successfully setup!");
	});

	// return 0;
}
