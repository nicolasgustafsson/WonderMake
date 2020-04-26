#pragma once
#include <soloud.h>
#include <soloud_wav.h>

class AudioManager : public System, Debugged
{
public:
	AudioManager();
protected:
	virtual void Debug() override;

	SoLoud::Soloud mySoloudEngine;

	std::optional<SoLoud::Wav> myBgm;
};

