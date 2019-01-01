#pragma once
#include "VertexTypes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "../Resources/ResourceManager.h"
#include "VertexAttributes.h"
#include "VertexBufferArray.h"

//everything needed to create a renderobject
struct SRenderObjectInfo
{
	std::filesystem::path VertexShaderPath;
	std::filesystem::path GeometryShaderPath = "";
	std::filesystem::path FragmentShaderPath;
	std::filesystem::path TexturePath;
	u32 VertexCount = 1;
	u32 GeometryType = GL_POINTS;
};

template<EVertexAttribute... TAttributes>
class RenderObject
{
public:
	RenderObject(const SRenderObjectInfo& aRenderObjectInfo);

	void Render();

	void BindTextures();

	template<EVertexAttribute TAttribute>
	void SetAttribute(const u32 aIndex, decltype(GetValueFromAttribute<TAttribute>()) aAttribute);

protected:
	ShaderProgram myShaderProgram;
	VertexBufferArray<TAttributes...> myVertexBufferArray;
	std::vector<std::shared_ptr<Texture>> myTextures;
	u32 myGeometryType;
	u32 myVertexCount;
};

template<EVertexAttribute... TAttributes>
template<EVertexAttribute TAttribute>
void RenderObject<TAttributes...>::SetAttribute(const u32 aIndex, decltype(GetValueFromAttribute<TAttribute>()) aAttribute)
{
	myVertexBufferArray.Set<TAttribute>(aIndex, aAttribute);
}

template<EVertexAttribute... TAttributes>
void RenderObject<TAttributes...>::BindTextures()
{
	for (auto[i, texture] : Utility::Enumerate(myTextures))
	{
		if (texture)
			texture->Bind(i);
	}
}

template<EVertexAttribute... TAttributes>
void RenderObject<TAttributes...>::Render()
{
	myShaderProgram.Activate();
	myVertexBufferArray.Render();

	glDrawArrays(myGeometryType, 0, myVertexCount);
}

template<EVertexAttribute... TAttributes>
RenderObject<TAttributes...>::RenderObject(const SRenderObjectInfo& aRenderObjectInfo)
	: myVertexBufferArray(aRenderObjectInfo.VertexCount)
	, myShaderProgram(aRenderObjectInfo.VertexShaderPath, aRenderObjectInfo.FragmentShaderPath, aRenderObjectInfo.GeometryShaderPath)
	, myGeometryType(aRenderObjectInfo.GeometryType)
{
	myVertexCount = aRenderObjectInfo.VertexCount;
	if (!aRenderObjectInfo.TexturePath.empty())
		myTextures.emplace_back(ResourceManager<Texture>::Get().GetResource(aRenderObjectInfo.TexturePath));
}

