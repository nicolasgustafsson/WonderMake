#include "pch.h"
#include "AudioMixingNodeGraph.h"
#include "Audio/AudioNodeTypes.h"


AudioMixingNodeGraph::AudioMixingNodeGraph(std::filesystem::path aPath)
	:  NodeGraph(aPath)
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
	Execute();
}

void AudioMixingNodeGraph::Compile()
{
	myCompiledNodeStack.clear();

	CompileNodeGraph(*myRootNode, myCompiledNodeStack);
}

void AudioMixingNodeGraph::Execute()
{
	//this sets up the busses and inputs/outputs
	for (size_t i = myCompiledNodeStack.size() - 1; i < myCompiledNodeStack.size(); i--)
	{
		auto&& compiledNode = myCompiledNodeStack[i];

		compiledNode.Node.NodeType.Execute(compiledNode.Node);
	}

	//this actually plays the busses. executing in "reverse" is required as soloud clears audio when beginning to play a bus.
	for (size_t i = 0; i < myCompiledNodeStack.size(); i++)
	{
		auto&& compiledNode = myCompiledNodeStack[i];

		compiledNode.Node.NodeType.ExecuteBackwards(compiledNode.Node);
	}
}

