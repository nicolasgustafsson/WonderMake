#pragma once
#include "NodeGraph/NodeTypes.h"
#include <filesystem>

class RenderTarget;

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
			AddSlot<SVector2u>(ESlotIo::Input, "Resolution");
			AddSlot<bool>(ESlotIo::Input, "Depth/Stencil buffer");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(struct SNode&) override;
	};

	struct SRenderDebugLines : public SNodeType<SRenderDebugLines>
	{
		SRenderDebugLines() : SNodeType("Render Debug Lines")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(struct SNode&) override;
	};

	struct SProcessRenderLayer : public SNodeType<SProcessRenderLayer>
	{
		SProcessRenderLayer() : SNodeType("Process Render Layer")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<std::string>(ESlotIo::Input, "In RenderLayer");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void ExecuteNode(struct SNode&) override;
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

		virtual void ExecuteNode(struct SNode&) override;
	};
}

namespace SlotColors
{
}