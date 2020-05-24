#include "pch.h"
#include "AudioMixingNodeGraph.h"
#include "NodeGraph/NodeTypes.h"
#include "Audio/AudioNodeTypes.h"


AudioMixingNodeGraph::AudioMixingNodeGraph()
{
	Name = "Audio Mixing Node Graph";

	myRootNode = &AddNode<NodeTypes::SAudioMixingResultNode>({ 1000, 400 });

	RegisterNode<NodeTypes::SAudioMixingResultNode>([](const SNode& aNode) {});
	RegisterNode<NodeTypes::SAudioMixNode>([](const SNode& aNode) {});
	RegisterNode<NodeTypes::SAudioSourceBusNode>([&](const SNode& aNode) {});
	RegisterNode<NodeTypes::SEchoFilter>([&](const SNode& aNode) {});

	myRootNode->IsImmortal = true;
}

void AudioMixingNodeGraph::Compile()
{
	myCompiledNodeStack.clear();

	CompileNodeGraph(*myRootNode, myCompiledNodeStack);

	Execute();
}

void AudioMixingNodeGraph::Execute()
{
	//this sets up the busses and inputs/outputs
	for (i32 i = myCompiledNodeStack.size() - 1; i >= 0; i--)
	{
		auto&& compiledNode = myCompiledNodeStack[i];

		compiledNode.Node.NodeType.Execute(compiledNode.Node);
	}

	//this actually plays the busses. executing in "reverse" is required as soloud clears audio when beginning to play a bus.
	for (i32 i = 0; i < myCompiledNodeStack.size(); i++)
	{
		auto&& compiledNode = myCompiledNodeStack[i];

		compiledNode.Node.NodeType.ExecuteBackwards(compiledNode.Node);
	}
}
