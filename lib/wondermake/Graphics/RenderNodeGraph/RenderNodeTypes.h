#pragma once

#include "NodeGraph/NodeTypes.h"

#include "wondermake-utility/Future.h"

#include <filesystem>
#include <memory>

class RenderTarget;

class ShaderProgram;

namespace NodeTypes
{
	struct SRenderResultNode : public SNodeType<SRenderResultNode>
	{
		SRenderResultNode() : SNodeType("RenderResultNode")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "Final Texture");
		}

		virtual void ExecuteNode(SNode& aNode) override;
	};

	struct SRenderTextureNode : public SNodeType<SRenderTextureNode>
	{
		SRenderTextureNode() : SNodeType("Create Rendertarget")
		{
			AddSlot<f32>(ESlotIo::Input, "RenderScale", 1.0f);
			AddSlot<bool>(ESlotIo::Input, "Depth/Stencil buffer");
			AddSlot<SColor>(ESlotIo::Input, "Clear Color", SColor::Black());
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(SNode&) override;
	};

	struct SRenderDebugLines : public SNodeType<SRenderDebugLines>
	{
		SRenderDebugLines() : SNodeType("Render Debug Lines")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(SNode&) override;
	};

	struct SProcessRenderLayer : public SNodeType<SProcessRenderLayer>
	{
		SProcessRenderLayer() : SNodeType("Process Render Layer")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<std::string>(ESlotIo::Input, "In RenderLayer");
			AddSlot<SRenderSettings>(ESlotIo::Input, "In render settings");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(SNode&) override;
	};

	struct SClearDepth : public SNodeType<SClearDepth>
	{
		SClearDepth() : SNodeType("Clear Depth Buffer")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<float>(ESlotIo::Input, "In Depth to set", 0.f);
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(SNode&) override;
	};

	struct SPostProcess : public SNodeType<SPostProcess>
	{
		SPostProcess() : SNodeType("Run post process effect")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In texture");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In texture 2");
			AddSlot<std::filesystem::path>(ESlotIo::Input, "In Post process shader path");
			AddSlot<SRenderSettings>(ESlotIo::Input, "In render settings");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(SNode&) override;

		Future<std::shared_ptr<ShaderProgram>> CreateProgramFuture;
	};
}