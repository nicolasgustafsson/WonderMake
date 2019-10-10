#include "pch.h"

#include "Game.h"
#include "Registry/Registry.h"

#include "Program/Engine.h"

#include "Message/Messages.h"

int main()
{
	Engine::Start([&] 
	{
		Registry::Register();
		Start();
		WmLog("Game successfully setup!");
	});

	return 0;
}
