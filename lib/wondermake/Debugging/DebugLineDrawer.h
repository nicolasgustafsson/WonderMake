#pragma once
#include "DebugLine.h"
#include "DebugLineRenderObject.h"
#include "Graphics/Shader.h"
#include "wondermake-base/System.h"
#include "Utilities/TimeKeeper.h"

template<typename TResource>
class ResourceSystem;

template class ResourceSystem<Shader<EShaderType::Vertex>>;
template class ResourceSystem<Shader<EShaderType::Fragment>>;
template class ResourceSystem<Shader<EShaderType::Geometry>>;

class DebugLineDrawer
	: public System<
		Policy::Set<
			PAdd<ResourceSystem<Shader<EShaderType::Vertex>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Fragment>>, PWrite>,
			PAdd<ResourceSystem<Shader<EShaderType::Geometry>>, PWrite>,
			PAdd<TimeKeeper, PRead>>,
		STrait::Set<
			STGui,
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
