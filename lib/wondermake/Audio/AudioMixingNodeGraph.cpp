#include "AudioMixingNodeGraph.h"
#include "Audio/AudioNodeTypes.h"

AudioMixingNodeGraph::AudioMixingNodeGraph(FilePath aPath)
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
