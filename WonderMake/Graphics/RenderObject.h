#pragma once
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexAttributes.h"
#include "Graphics/VertexBufferArray.h"
#include "Graphics/VertexTypes.h"

#include "Resources/ResourceSystem.h"

#include "System/SystemPtr.h"
#include "OpenGLFacade.h"

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

class BaseRenderObject
{
public:
	void Render();

	friend class RenderCommand;
protected:

	virtual void RenderInternal() = 0;
	i32 myRenderOrder = 0;
};

class RenderCommand : public NonCopyable
{
public:
	RenderCommand(BaseRenderObject& aRenderObject, const u32 aRenderOrder) : myRenderObject(aRenderObject), myRenderOrder(aRenderOrder) {}

	RenderCommand& operator=(RenderCommand&&) = default;
	RenderCommand(RenderCommand&&) = default;

	void Execute();

	inline [[nodiscard]] bool operator==(const RenderCommand& aRhs) const noexcept
	{
		return myRenderOrder == aRhs.myRenderOrder;
	}

	inline [[nodiscard]] auto operator<=>(const RenderCommand& aRhs) const noexcept
	{
		return myRenderOrder <=> aRhs.myRenderOrder;
	}

private:
	std::reference_wrapper<BaseRenderObject> myRenderObject;
	i32 myRenderOrder;
};

template<EVertexAttribute... TAttributes>
class RenderObject : public BaseRenderObject
{
public:
	RenderObject(const SRenderObjectInfo& aRenderObjectInfo);

	void SetTexture(std::filesystem::path aTexturePath);

	void BindTextures();

	void SetRenderCount(u32 aRenderCount);

	template<EVertexAttribute TAttribute>
	void SetAttribute(const u32 aIndex, decltype(GetValueFromAttribute<TAttribute>()) aAttribute);

protected:

	virtual void RenderInternal() override;

	ShaderProgram myShaderProgram;
	VertexBufferArray<TAttributes...> myVertexBufferArray;
	std::vector<ResourceProxy<Texture>> myTextures;
	std::optional<u32> myRenderCount;
	u32 myGeometryType;
	u32 myVertexCount;
};

template<EVertexAttribute... TAttributes>
void RenderObject<TAttributes...>::RenderInternal()
{
	const bool shaderProgramActivated = myShaderProgram.Activate();

	if (!shaderProgramActivated)
		return;

	myVertexBufferArray.Render();
	BindTextures();

	const u32 renderCount = myRenderCount ? *myRenderCount : myVertexCount;

	SystemPtr<OpenGLFacade>()->DrawArrays(myGeometryType, 0, renderCount);
}

template<EVertexAttribute... TAttributes>
void RenderObject<TAttributes...>::SetTexture(std::filesystem::path aTexturePath)
{
	myTextures.clear();
	if (!aTexturePath.empty())
		myTextures.emplace_back(SystemPtr<ResourceSystem<Texture>>()->GetResource(aTexturePath));
}

template<EVertexAttribute... TAttributes>
void RenderObject<TAttributes...>::SetRenderCount(u32 aRenderCount)
{
	//[Nicos]: We might want to resize the vertexbuffer here in the future
	assert(aRenderCount < myVertexCount && "You can't render more vertices than there are in the vertex buffer!");
	myRenderCount = aRenderCount;
}

template<EVertexAttribute... TAttributes>
template<EVertexAttribute TAttribute>
void RenderObject<TAttributes...>::SetAttribute(const u32 aIndex, decltype(GetValueFromAttribute<TAttribute>()) aAttribute)
{
	myVertexBufferArray.template Set<TAttribute>(aIndex, aAttribute);
}

template<EVertexAttribute... TAttributes>
void RenderObject<TAttributes...>::BindTextures()
{
	for (auto[i, texture] : Utility::Enumerate(myTextures))
	{
		if (texture)
			texture->Bind(static_cast<u32>(i));
	}
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

