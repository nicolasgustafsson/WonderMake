#include "pch.h"
#include "BuffGiverRenderObject.h"

BuffGiverRenderObject::BuffGiverRenderObject()
	:RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/BuffGiverVertex.vert"
		,	std::filesystem::current_path() / "Shaders/Geometry/BuffGiverGeometry.geom"
		,	std::filesystem::current_path() / "Shaders/Fragment/BuffGiverFragment.frag"
		,	{}
		,	1 })
{

}
