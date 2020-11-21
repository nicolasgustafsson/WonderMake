#include "pch.h"
#include "BuffGiverRenderObject.h"

BuffGiverRenderObject::BuffGiverRenderObject()
	:RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/BuffGiver.vert"
		,	std::filesystem::current_path() / "Shaders/Geometry/BuffGiver.geom"
		,	std::filesystem::current_path() / "Shaders/Fragment/BuffGiver.frag"
		,	{}
		,	1 })
{

}
