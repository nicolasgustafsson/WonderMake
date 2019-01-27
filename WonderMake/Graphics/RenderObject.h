#pragma once
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexAttributes.h"
#include "Graphics/VertexBufferArray.h"
#include "Graphics/VertexTypes.h"

#include "Resources/ResourceSystem.h"

#include "System/SystemPtr.h"

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
	std::vector<ResourceProxy<Texture>> myTextures;
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
	BindTextures();

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
		myTextures.emplace_back(SystemPtr<ResourceSystem<Texture>>()->GetResource(aRenderObjectInfo.TexturePath));
}

