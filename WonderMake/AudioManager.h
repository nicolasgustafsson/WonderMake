#pragma once
#include <soloud.h>

class AudioManager : public System, Debugged
{
public:
	AudioManager();
protected:
	virtual void Debug() override;

	SoLoud::Soloud mySoloudEngine;
};

