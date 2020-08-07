#pragma once
class RenderCommandProcessor : public System
{
public:
	RenderCommandProcessor();
	void AddToQueue(RenderCommand aCommand);

	void Tick() noexcept override;

private:
	void ProcessQueue();

	std::vector<RenderCommand> myRenderCommands;
};

