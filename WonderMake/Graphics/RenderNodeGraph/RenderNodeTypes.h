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

	struct SProcessRenderCommandQueue : public SNodeType<SProcessRenderCommandQueue>
	{
		SProcessRenderCommandQueue() : SNodeType("Process Render Command queue")
		{
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Input, "In rendertarget");
			AddSlot<std::shared_ptr<RenderTarget>>(ESlotIo::Output, "Out rendertarget");
		}

		void ExecuteNodeLeftToRight(struct SNode&) override;
	};

	//
	//struct SEchoFilter : public SNodeType<SEchoFilter>
	//{
	//	SEchoFilter() : SNodeType("Echo Filter")
	//	{
	//		AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Input");
	//		AddSlot<f32>(ESlotIo::Input, "Delay", 0.5f);
	//		AddSlot<f32>(ESlotIo::Input, "Decay", 0.5f);
	//		AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Output");
	//	}
	//
	//	virtual void PrepareNode(SNode& aNode) override;
	//
	//	virtual void ExecuteNodeLeftToRight(SNode& aNode) override;
	//};
	//
	//struct SGetSoundEffect : public SNodeType<SGetSoundEffect>
	//{
	//	SGetSoundEffect() : SNodeType("Get sound effect")
	//	{
	//		AddSlot<std::filesystem::path>(ESlotIo::Input, "Sound effect name"); //[Nicos]: TODO make this a path
	//		AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Output");
	//	}
	//
	//	virtual void PrepareNode(SNode& aNode) override;
	//};
}

namespace SlotColors
{
	//template<>
	//inline ImColor GetColor<SoLoud::AudioSource*>()
	//{
	//	return ImColor(255, 192, 32, 255);
	//	//return ImColor(128, 192, 255, 255); // teal
	//}
}