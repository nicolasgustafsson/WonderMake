#pragma once

#include "DebugLine.h"
#include "DebugLineRenderObject.h"
#include "Graphics/Shader.h"
#include "wondermake-base/System.h"

template<typename TResource>
class ResourceSystem;
class TimeKeeperSingleton;

template class ResourceSystem<Shader<EShaderType::Vertex>>;
template class ResourceSystem<Shader<EShaderType::Fragment>>;
template class ResourceSystem<Shader<EShaderType::Geometry>>;

class DebugLineDrawer
	: public System<
		Policy::Set<
			PAdd<ResourceSystem<Shader<EShaderType::Vertex>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Fragment>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Geometry>>, PWrite>,
			PAdd<TimeKeeperSingleton, PRead>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	DebugLineDrawer() noexcept;

	void Render();
	void Update();

private:

	void OnGotDebugLineMessage(const SDebugLineMessage& aDebugLineMessage);

	MessageSubscriber mySubscriber;
	std::optional<DebugLineRenderObject> myRenderObject;
	std::vector<SDebugLine> myDebugLines;
};
