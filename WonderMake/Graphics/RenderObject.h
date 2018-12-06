#pragma once
#include "VertexTypes.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "../Resources/ResourceManager.h"

//everything needed to create a renderobject
template<typename VertexType = SGenericVertex, u32 VertexCount = 1>
struct SRenderObjectInfo
{
	std::filesystem::path VertexShaderPath;
	std::filesystem::path GeometryShaderPath;
	std::filesystem::path FragmentShaderPath; //will make MaterialPath later
	std::filesystem::path TexturePath; //will make MaterialPath later

	std::array<VertexType, VertexCount> VertexData;
};

template<typename VertexType = SGenericVertex, u32 VertexCount = 1>
class RenderObject
{
public:
	RenderObject(const SRenderObjectInfo<VertexType, VertexCount>& RenderObjectInfo);

	void Render();

private:
	Material myMaterial;
	ShaderProgram myShaderProgram;
	VertexBuffer<VertexType, VertexCount> myVertexBuffer;
};

template<typename VertexType /*= SGenericVertex*/, u32 VertexCount /*= 1*/>
void RenderObject<VertexType, VertexCount>::Render()
{
	myVertexBuffer.Bind();
	myShaderProgram.Activate();
	myMaterial.BindTextures();

	glDrawArrays(GL_POINTS, 0, 1);
}

template<typename VertexType /*= SGenericVertex*/, u32 VertexCount /*= 1*/>
RenderObject<VertexType, VertexCount>::RenderObject(const SRenderObjectInfo<VertexType, VertexCount>& RenderObjectInfo)
	: myVertexBuffer(RenderObjectInfo.VertexData)
	, myMaterial({}, {ResourceManager<Texture>::Get().GetResource(RenderObjectInfo.TexturePath)})
	, myShaderProgram(RenderObjectInfo.VertexShaderPath, RenderObjectInfo.FragmentShaderPath, RenderObjectInfo.GeometryShaderPath)
{

}

