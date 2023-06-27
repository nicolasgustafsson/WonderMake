#include "SoundEffectNodeGraph.h"
#include "Audio/AudioNodeTypes.h"

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
