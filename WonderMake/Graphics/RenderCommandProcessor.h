#pragma once
class RenderCommandProcessor : public System
{
public:
	RenderCommandProcessor();
	void AddToQueue(RenderCommand aCommand);
	void ProcessQueue();
	void ClearQueue();

private:

	std::vector<RenderCommand> myRenderCommands;
};

