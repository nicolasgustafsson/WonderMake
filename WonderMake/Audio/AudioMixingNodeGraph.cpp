#include "pch.h"
#include "AudioMixingNodeGraph.h"
#include "Audio/AudioNodeTypes.h"

_REGISTER_SYSTEM_IMPL(ResourceSystem<AudioMixingNodeGraph>, AudioMixingResource);

AudioMixingNodeGraph::AudioMixingNodeGraph(std::filesystem::path aPath)
	:  NodeGraph(std::move(aPath))
{
	Load();
}

void AudioMixingNodeGraph::RegisterNodes()
{
	RegisterRootNode<NodeTypes::SAudioMixingResultNode>();
	RegisterNode<NodeTypes::SAudioMixNode>();
	RegisterNode<NodeTypes::SAudioSourceBusNode>();
	RegisterNode<NodeTypes::SEchoFilter>();
}

void AudioMixingNodeGraph::PostLoad()
{
	Compile();
}
