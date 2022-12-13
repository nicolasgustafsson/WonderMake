#pragma once
#include "Graphics/RenderObject.h"

class RenderLayer : public NonCopyable, NonMovable
{
public:
	RenderLayer();
	RenderHandle AddToQueue(RenderCommand aCommand);
	void ProcessQueue();
	void ClearQueue();
	void RemoveObjectFromQueue(const u64 aId);
private:
	[[nodiscard]] u64 GetNextRenderId() noexcept;

	std::vector<RenderCommand> myRenderCommands;

	//[Nicos]: With an underscore because you are supposed to use GetNextRenderId and I've already fucked up
	u64 _myRenderId = 0;
};

class RenderCommandProcessor
	: public System<
		Policy::Set<>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	RenderLayer& GetRenderLayer(std::string_view aRenderLayerName);

	void Clear();

private:
	std::unordered_map<std::string, RenderLayer> myRenderLayers;
};

