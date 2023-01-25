#include "pch.h"
#include "SoundEffectNodeGraph.h"
#include "Audio/AudioNodeTypes.h"

#include "wondermake-base/SystemGlobal.h"

_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<SoundEffectNodeGraph>, SoundEffectNodeGraphResource);

SoundEffectNodeGraph::SoundEffectNodeGraph(std::filesystem::path aPath)
	: NodeGraph(std::move(aPath))
{
	Load();
}

void SoundEffectNodeGraph::RegisterNodes()
{
	RegisterRootNode<NodeTypes::SSoundEffectResultNode>();
	RegisterNode<NodeTypes::SEchoFilter>();
	RegisterNode<NodeTypes::SGetSoundEffect>();
}

void SoundEffectNodeGraph::PostLoad()
{
	Compile();
}
