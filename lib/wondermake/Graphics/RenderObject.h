#pragma once

#include "Graphics/RenderHandle.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"

#include "Resources/ResourceProxy.h"

#include "Utilities/Container/Container.h"

#include "wondermake-ui/OpenGLFacade.h"
#include "wondermake-ui/VertexAttributes.h"
#include "wondermake-ui/VertexBufferArray.h"
#include "wondermake-ui/VertexTypes.h"

#include "wondermake-base/SystemPtr.h"


//everything needed to create a renderobject
struct SRenderObjectInfo
{
	ResourceSystem<Shader<EShaderType::Vertex>>& VsSystem;
	ResourceSystem<Shader<EShaderType::Fragment>>& FsSystem;
	ResourceSystem<Shader<EShaderType::Geometry>>& GsSystem;
	std::filesystem::path VertexShaderPath;
	std::filesystem::path GeometryShaderPath = "";
	std::filesystem::path FragmentShaderPath;
	ResourceProxy<Texture> TextureAsset;
	u32 VertexCount = 1;
	u32 GeometryType = GL_POINTS;
};

class BaseRenderObject
{
public:
	virtual ~BaseRenderObject();
	void Render();
	void RenderImmediate();

	friend class RenderCommand;

	void SetRenderLayer(const std::string_view& aRenderLayer) { myRenderLayer = aRenderLayer; };
	void SetRenderOrder(i32 aRenderOrder) noexcept { myRenderOrder = aRenderOrder; }
protected:

	virtual void RenderInternal() = 0;
	i32 myRenderOrder = 0;

	std::string_view myRenderLayer = "Default";

	std::optional<RenderHandle> myCurrentRenderHandle;
};

class RenderCommand : public NonCopyable
{
public:
	RenderCommand(BaseRenderObject& aRenderObject, const u32 aRenderOrder) : myRenderObject(aRenderObject), myRenderOrder(aRenderOrder) {}

	RenderCommand& operator=(RenderCommand&&) = default;
	RenderCommand(RenderCommand&&) = default;

	void Execute();

	inline void SetRenderId(const u64 aRenderId) noexcept { myRenderId = aRenderId; }
	inline [[nodiscard]] u64 GetRenderId() const noexcept { return myRenderId; }

	inline [[nodiscard]] auto operator<=>(const RenderCommand& aRhs) const noexcept
	{
		return myRenderOrder <=> aRhs.myRenderOrder;
	}


	inline [[nodiscard]] bool operator==(const RenderCommand& aRhs) const noexcept = default;

private:
	std::reference_wrapper<BaseRenderObject> myRenderObject;
	i32 myRenderOrder;
	u64 myRenderId;
};

template<EVertexAttribute... TAttributes>
class RenderObject : public BaseRenderObject
{
public:
	RenderObject(const SRenderObjectInfo& aRenderObjectInfo);

	void BindTextures();

	void SetRenderCount(const u32 aRenderCount);

	template<EVertexAttribute TAttribute>
	void SetAttribute(const u32 aIndex, decltype(GetValueFromAttribute<TAttribute>()) aAttribute);

	template<typename TProperty>
	void SetProperty(std::string_view aName, TProperty aProperty);

protected:
	virtual void RenderInternal() override;

	ShaderProgram myShaderProgram;
	VertexBufferArray<TAttributes...> myVertexBufferArray;
	Container<ResourceProxy<Texture>, Indexable> myTextures;
	std::optional<u32> myRenderCount;
	u32 myGeometryType;
	u32 myVertexCount;
};

template<EVertexAttribute... TAttributes>
template<typename TProperty>
void RenderObject<TAttributes...>::SetProperty(std::string_view aName, TProperty aProperty)
{
	myShaderProgram.SetProperty(aName, aProperty);
}

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
void RenderObject<TAttributes...>::SetRenderCount(const u32 aRenderCount)
{
	if (aRenderCount > myVertexCount)
	{
		myVertexCount = aRenderCount;

		//[Nicos]: reconstruct vertex buffer
		myVertexBufferArray.~VertexBufferArray();
		new (&myVertexBufferArray)VertexBufferArray<TAttributes...>(myVertexCount);
	}
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
	, myShaderProgram(aRenderObjectInfo.VsSystem, aRenderObjectInfo.FsSystem, aRenderObjectInfo.GsSystem, aRenderObjectInfo.VertexShaderPath, aRenderObjectInfo.FragmentShaderPath, aRenderObjectInfo.GeometryShaderPath)
	, myGeometryType(aRenderObjectInfo.GeometryType)
{
	myVertexCount = aRenderObjectInfo.VertexCount;
	if (aRenderObjectInfo.TextureAsset)
		myTextures.Add(aRenderObjectInfo.TextureAsset);
}

