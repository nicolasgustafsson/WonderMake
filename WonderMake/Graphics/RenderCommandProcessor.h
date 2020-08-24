#pragma once

class RenderLayer : public NonCopyable
{
public:
	void AddToQueue(RenderCommand aCommand);
	void ProcessQueue();
	void ClearQueue();
private:
	std::vector<RenderCommand> myRenderCommands;
};

class RenderCommandProcessor : public System
{
public:
	RenderLayer& GetRenderLayer(std::string& aRenderLayerName);

	void Clear();

private:
	std::unordered_map<std::string, RenderLayer> myRenderLayers;
};

