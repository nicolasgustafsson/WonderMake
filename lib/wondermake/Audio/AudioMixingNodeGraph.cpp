#include "AudioMixingNodeGraph.h"
#include "Audio/AudioNodeTypes.h"

#include "wondermake-base/SystemGlobal.h"

_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<AudioMixingNodeGraph>, AudioMixingResource);

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
