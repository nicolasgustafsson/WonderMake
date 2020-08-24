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

		virtual void ExecuteNodeLeftToRight(SNode& aNode) override;
	};

	struct SRenderTextureNode : public SNodeType<SRenderTextureNode>
	{
		SRenderTextureNode() : SNodeType("Create Rendertarget")
		{
			AddSlot<SVector2u>(ESlotIo::Input, "Resolution");
			AddSlot<bool>(ESlotIo::Input, "Depth/Stencil buffer");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		virtual void PrepareNode(SNode& aNode) override;

		void ExecuteNodeRightToLeft(struct SNode&) override;
	};

	struct SProcessRenderLayer : public SNodeType<SProcessRenderLayer>
	{
		SProcessRenderLayer() : SNodeType("Process Render Layer")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<std::string>(ESlotIo::Input, "In RenderLayer");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		void ExecuteNodeLeftToRight(struct SNode&) override;
	};
}

namespace SlotColors
{
}