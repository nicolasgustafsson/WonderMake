#pragma once

#include "Graphics/RenderHandle.h"

#include "Utilities/Container/Container.h"

#include "wondermake-ui/OpenGLFacade.h"
#include "wondermake-ui/ShaderProgram.h"
#include "wondermake-ui/ShaderResourceSystem.h"
#include "wondermake-ui/TextureResourceSystem.h"
#include "wondermake-ui/VertexAttributes.h"
#include "wondermake-ui/VertexBufferArray.h"
#include "wondermake-ui/VertexTypes.h"

#include "wondermake-base/SystemPtr.h"

//everything needed to create a renderobject
struct SRenderObjectInfo
{
	ShaderResourceSystem&				ShaderSystem;
	std::shared_ptr<ShaderProgram>		ShaderProgram;
	FileResourcePtr<TextureResource>	TextureAsset;
	u32									VertexCount = 1;
	u32									GeometryType = GL_POINTS;
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
	RenderObject(const SRenderObjectInfo& aRenderObjectInfo)
		: myShaderSystem(aRenderObjectInfo.ShaderSystem)
		, myShaderProgram(std::move(aRenderObjectInfo.ShaderProgram))
		, myVertexBufferArray(aRenderObjectInfo.VertexCount)
		, myGeometryType(aRenderObjectInfo.GeometryType)
		, myVertexCount(aRenderObjectInfo.VertexCount)
	{
		if (aRenderObjectInfo.TextureAsset)
			myTextures.Add(aRenderObjectInfo.TextureAsset.ToRef());
	}

	void BindTextures()
	{
		SystemPtr<TextureResourceSystem> texResSys;

		for (auto [i, texture] : Utility::Enumerate(myTextures))
			texResSys->Bind(texture, static_cast<u32>(i));
	}

	void SetRenderCount(const u32 aRenderCount)
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

	template<EVertexAttribute TAttribute>
	void SetAttribute(const u32 aIndex, decltype(GetValueFromAttribute<TAttribute>()) aAttribute)
	{
		myVertexBufferArray.template Set<TAttribute>(aIndex, aAttribute);
	}

	template<typename TProperty>
	void SetProperty(const char* aName, TProperty aProperty)
	{
		myShaderSystem.SetProgramProperty(myShaderProgram, aName, aProperty);
	}

protected:
	virtual void RenderInternal() override
	{
		const bool shaderProgramActivated = myShaderSystem.ActivateProgram(myShaderProgram);

		if (!shaderProgramActivated)
			return;

		myVertexBufferArray.Render();
		BindTextures();

		const u32 renderCount = myRenderCount ? *myRenderCount : myVertexCount;

		SystemPtr<OpenGLFacade>()->DrawArrays(myGeometryType, 0, renderCount);
	}

	ShaderResourceSystem&									myShaderSystem;
	std::shared_ptr<ShaderProgram>							myShaderProgram;
	VertexBufferArray<TAttributes...>						myVertexBufferArray;
	Container<FileResourceRef<TextureResource>, Indexable>	myTextures;
	std::optional<u32>										myRenderCount;
	u32														myGeometryType;
	u32														myVertexCount;
};
