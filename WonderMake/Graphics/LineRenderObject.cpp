#include "stdafx.h"
#include "LineRenderObject.h"


LineRenderObject::LineRenderObject()
	:RenderObject(SRenderObjectInfo
		{	std::filesystem::current_path() / "Shaders/Vertex/LineVertex.vert"
		,	"" //geometry shader
		,	std::filesystem::current_path() / "Shaders/Fragment/LineFragment.frag"
		,	"" //texture
		,	2
		,	GL_LINES})
{

}