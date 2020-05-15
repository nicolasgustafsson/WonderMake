#include "pch.h"
#include "AudioMixingNodeGraph.h"
#include "NodeGraph/NodeTypes.h"
#include "Audio/AudioNodeTypes.h"

AudioMixingNodeGraph::AudioMixingNodeGraph()
{
	AddNode<NodeTypes::SAudioMixingResultNode>({1000, 400});
	RegisterNode<NodeTypes::SAudioMixNode>();
	RegisterNode<NodeTypes::SAudioFilterNode>();
	RegisterNode<NodeTypes::SAudioSourceBusNode>();
	RegisterNode<NodeTypes::SAddFloatsNode>();
	RegisterNode<NodeTypes::SPickColorNode>();

	Name = "Audio Mixing Node Graph";
}
