#include "pch.h"
#include "SoundEffectNodeGraph.h"
#include "Audio/AudioNodeTypes.h"

SoundEffectNodeGraph::SoundEffectNodeGraph(std::filesystem::path aPath)
	: NodeGraph(aPath)
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
