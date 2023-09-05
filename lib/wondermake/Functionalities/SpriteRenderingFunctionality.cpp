#include "SpriteRenderingFunctionality.h"

#include "Functionalities/TransformFunctionality.h"

#include "Graphics/SpriteRenderObject.h"

#include "wondermake-ui/ShaderResourceSystem.h"
#include "wondermake-ui/TextureResourceSystem.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/Component.h"
#include "wondermake-base/ConfigurationSystem.h"

#include <iostream>
#include <optional>

WM_REGISTER_COMPONENT(SSpriteComponent);
WM_REGISTER_FUNCTIONALITY(SpriteRenderingFunctionality);

struct SSpriteComponent 
	: public STraitedComponent<
		STrait::Set<
			STWonderMake>>
{
	std::optional<SpriteRenderObject>	RenderObject;
	bool								IsHidden = false;
	Future<void>						OnInitialized;
};

SpriteRenderingFunctionality::SpriteRenderingFunctionality()
{
	auto& spriteComponent = Get<SSpriteComponent>();

	spriteComponent.OnInitialized = Get<ShaderResourceSystem>()
		.CreateProgram(FilePath(FilePath::EFolder::Bin, "shaders/sprite.vert"), FilePath(FilePath::EFolder::Bin, "shaders/sprite.frag"), FilePath(FilePath::EFolder::Bin, "shaders/sprite.geom"))
		.ThenApply(GetExecutor(), FutureApplyResult([this](auto aProgram)
			{
				Get<SSpriteComponent>().RenderObject.emplace(Get<ShaderResourceSystem>(), std::move(aProgram));

				return MakeCompletedFuture<void>();
			}));
}

void SpriteRenderingFunctionality::Initialize(SInitializationInfo aInfo)
{
	SetTexture(std::move(aInfo.FilePath));

	SetScale(aInfo.Scale);
	SetColor(aInfo.Color);
	SetOrigin(aInfo.Origin);
	SetRenderLayer(aInfo.RenderLayer);
	SetRenderOrder(aInfo.RenderOrder);
}

void SpriteRenderingFunctionality::Tick()
{
	auto& spriteComponent = Get<SSpriteComponent>();
	if (!spriteComponent.RenderObject || spriteComponent.IsHidden)
		return;

	const auto& transform = Get<TransformFunctionality2D>();

	spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Position>(0, transform.GetPosition());
	spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Rotation>(0, transform.GetRotation());
	spriteComponent.RenderObject->Render();
}

void SpriteRenderingFunctionality::SetTexture(const FilePath& aFilePath)
{
	Get<TextureResourceSystem>()
		.GetTexture(aFilePath)
		.ThenRun(GetExecutor(), FutureRunResult([this](auto aRef)
			{
				SetTexture(std::move(aRef));
			}))
		.Detach();
}

[[nodiscard]] SVector2u SpriteRenderingFunctionality::GetTextureSize() const noexcept
{
	auto& spriteComponent = Get<SSpriteComponent>();

	if (!spriteComponent.RenderObject)
		return SVector2u::Zero();

	auto texture = spriteComponent.RenderObject->GetTexture(0);

	if (!texture)
		return SVector2u::Zero();

	return SVector2u(
		static_cast<u32>(texture->Size().X),
		static_cast<u32>(texture->Size().Y));
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	Get<SSpriteComponent>()
		.OnInitialized
		.ThenRun(GetExecutor(), FutureRunResult([this, aScale]()
			{
				Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
			}));
}

void SpriteRenderingFunctionality::SetOrigin(const SVector2f aOrigin)
{
	Get<SSpriteComponent>()
		.OnInitialized
		.ThenRun(GetExecutor(), FutureRunResult([this, aOrigin]()
			{
				Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Origin>(0, aOrigin);
			}));
}

void SpriteRenderingFunctionality::SetColor(const SColor aColor)
{
	Get<SSpriteComponent>()
		.OnInitialized
		.ThenRun(GetExecutor(), FutureRunResult([this, aColor]()
			{
				Get<SSpriteComponent>().RenderObject->SetColor(aColor);
			}));
}

void SpriteRenderingFunctionality::SetRenderLayer(std::string_view aRenderLayer)
{
	Get<SSpriteComponent>()
		.OnInitialized
		.ThenRun(GetExecutor(), FutureRunResult([this, aRenderLayer]()
			{
				Get<SSpriteComponent>().RenderObject->SetRenderLayer(aRenderLayer);
			}));
}

void SpriteRenderingFunctionality::SetRenderOrder(i32 aRenderOrder)
{
	Get<SSpriteComponent>()
		.OnInitialized
		.ThenRun(GetExecutor(), FutureRunResult([this, aRenderOrder]()
			{
				Get<SSpriteComponent>().RenderObject->SetRenderOrder(aRenderOrder);
			}));
}

void SpriteRenderingFunctionality::Hide() noexcept
{
	Get<SSpriteComponent>().IsHidden = true;
}

void SpriteRenderingFunctionality::Show() noexcept
{
	Get<SSpriteComponent>().IsHidden = false;
}

void SpriteRenderingFunctionality::SetTexture(FileResourceRef<TextureResource>&& aTexture)
{
	const auto setTexture = [this](auto aTexture)
	{
		Get<SSpriteComponent>()
			.OnInitialized
			.ThenRun(GetExecutor(), FutureRunResult([this, texture = std::move(aTexture)]() mutable
				{
					Get<SSpriteComponent>().RenderObject->SetTexture(std::move(texture));
				}));
	};

	if (aTexture->IsValid())
	{
		setTexture(std::move(aTexture));

		return;
	}
	
	Get<TextureResourceSystem>()
		.GetTexture(Get<ConfigurationSystem>().Get<FilePath>(ConfigurationEngine::MissingTexturePath, FilePath()))
		.ThenRun(GetExecutor(), FutureRunResult(setTexture))
		.Detach();
}
