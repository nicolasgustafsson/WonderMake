#pragma once
#include <queue>

//[Nicos]: Basically stolen from my TGA engine project :P
//[Nicos]: We *do* use a separate thread for this, but it's spending most of its time snoozing.

class FileWatcher 
	: public System
{
public:
	FileWatcher();

	void UpdateFileChanges();

private:
	void Watch();
	std::thread myThread;

	std::atomic<bool> myShouldStop;
	std::unordered_map<std::string, float> lastTime;

	std::mutex myMutex;
	std::queue<std::string> myQueuedChanges;

	float myMinimumTimeDifference;
};

