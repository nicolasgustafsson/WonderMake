
#include "Game.h"

#include "Program/Engine.h"

#include "Message/Messages.h"

int main()
{
	Engine::Start([&] 
	{
		Start();
		WmLog("Game successfully setup!");
	});

	return 0;
}
