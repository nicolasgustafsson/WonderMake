#pragma once
#include "Graphics/RenderObject.h"
#include "Graphics/Shader.h"

template<typename TResource>
class ResourceSystem;

template class ResourceSystem<Shader<EShaderType::Vertex>>;
template class ResourceSystem<Shader<EShaderType::Fragment>>;
template class ResourceSystem<Shader<EShaderType::Geometry>>;

struct SDebugLine;

class DebugLineRenderObject
	: public RenderObject
		< EVertexAttribute::Position
		, EVertexAttribute::Color>
{
public:
	DebugLineRenderObject(ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem, ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem, ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem, const u32 aMaxLineCount);

	void SetLine(u32 aLineIndex, const SDebugLine& aDebugLine);
	void SetLineCount(const u32 aLineCount);
};

