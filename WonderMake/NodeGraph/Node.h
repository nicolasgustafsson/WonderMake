#pragma once
#include "NodeGraph/SlotColors.h"
#include "NodeGraph/InputSlotEdits.h"
#include <any>
#include <unordered_map>

struct SSlotTypeBase;

struct SConnection
{
	void* InputNodePointer = nullptr;
	void* OutputNodePointer = nullptr;
	const char* InputSlotName = nullptr;
	const char* OutputSlotName = nullptr;

	struct SInputSlotInstanceBase* InputSlot = nullptr;
	struct SOutputSlotInstanceBase* OutputSlot = nullptr;

	~SConnection();

	ImColor Color;
};


enum class ESlotIo
{
	Input,
	Output
};

struct SSlotInstanceBase
{
	SSlotInstanceBase(const SSlotTypeBase& aSlotType)
		: SlotType(aSlotType)
	{

	}
	virtual ~SSlotInstanceBase() {};

	const SSlotTypeBase& SlotType;
	virtual void Inspect() {};
	virtual bool HasConnection() const = 0;
};

struct SOutputSlotInstanceBase : public SSlotInstanceBase
{
	SOutputSlotInstanceBase(const SSlotTypeBase& aSlotType)
		: SSlotInstanceBase(aSlotType) {}

	std::vector<SConnection*> Connections;
	virtual void Inspect() override final {};
	virtual bool HasConnection() const override final
	{
		return Connections.size() != 0;
	}
};

struct SInputSlotInstanceBase : public SSlotInstanceBase
{
	SInputSlotInstanceBase(const SSlotTypeBase& aSlotType)
		: SSlotInstanceBase(aSlotType) {}

	SConnection* Connection = nullptr;

	virtual bool HasConnection() const override final
	{
		return Connection != nullptr;
	}
};

template<typename T>
struct SInputSlotInstance : public SInputSlotInstanceBase
{
	SInputSlotInstance(const SSlotTypeBase& aSlotType)
		: SInputSlotInstanceBase(aSlotType) {}

	virtual void Inspect()
	{
		InputSlotEdits::template EditInputSlot<T>(EditableValue);
	}

	T EditableValue = {};
};

template<typename T>
struct SOutputSlotInstance : public SOutputSlotInstanceBase
{
	SOutputSlotInstance(const SSlotTypeBase& aSlotType)
		: SOutputSlotInstanceBase(aSlotType) {}

	T OutputValue = {};
};


struct SSlotTypeBase abstract
{
	virtual ~SSlotTypeBase() {};

	std::string Name = "";

	virtual ImColor GetColor() const = 0;
	virtual size_t GetTypeHash() const = 0;

	virtual std::unique_ptr<SInputSlotInstanceBase> CreateInputSlotInstance() const = 0;
	virtual std::unique_ptr<SOutputSlotInstanceBase> CreateOutputSlotInstance() const = 0;

	bool IsCompatibleWith(const SSlotTypeBase& aOther) const
	{
		return GetTypeHash() == aOther.GetTypeHash();
	}
};

template<typename TSlotType>
struct SSlotType : public SSlotTypeBase
{
	virtual ImColor GetColor() const override
	{
		return SlotColors::template GetColor<TSlotType>();
	}

	virtual size_t GetTypeHash() const override
	{
		return typeid(TSlotType).hash_code();
	}

	virtual std::unique_ptr<SInputSlotInstanceBase> CreateInputSlotInstance() const override
	{
		return std::make_unique<SInputSlotInstance<TSlotType>>(( *this ));
	}

	virtual std::unique_ptr<SOutputSlotInstanceBase> CreateOutputSlotInstance() const override
	{
		return std::make_unique<SOutputSlotInstance<TSlotType>>(( *this ));
	}
};

struct SNodeTypeBase
{
	SNodeTypeBase(std::string aTitle)
		: Title(aTitle) {}

	virtual ~SNodeTypeBase() {};

	std::string Title = "";

	std::vector<std::unique_ptr<SSlotTypeBase>> InputSlots = {};
	std::vector<std::unique_ptr<SSlotTypeBase>> OutputSlots = {};

	std::vector<std::unique_ptr<SInputSlotInstanceBase>> CreateInputSlotInstances() const
	{
		std::vector<std::unique_ptr<SInputSlotInstanceBase>> slotInstances;

		for (auto&& inputSlot : InputSlots)
		{
			slotInstances.push_back(inputSlot->CreateInputSlotInstance());
		}

		return slotInstances;
	}

	std::vector<std::unique_ptr<SOutputSlotInstanceBase>> CreateOutputSlotInstances() const
	{
		std::vector<std::unique_ptr<SOutputSlotInstanceBase>> slotInstances;

		for (auto&& outputSlot : OutputSlots)
		{
			slotInstances.push_back(outputSlot->CreateOutputSlotInstance());
		}

		return slotInstances;
	}

	virtual void Execute(struct SNode&) {}

	virtual void ExecuteBackwards(struct SNode&) {}
};

template<typename TNodeType>
struct SNodeType : public SNodeTypeBase
{
	SNodeType(std::string aTitle)
		: SNodeTypeBase(aTitle)
	{

	}

	static TNodeType StaticObject;

protected:
	template <typename T>
	void AddSlot(ESlotIo aIoType, std::string aSlotName)
	{
		SSlotType<T> slot = {};

		slot.Name = aSlotName;

		switch (aIoType)
		{
		case ESlotIo::Input:
			InputSlots.push_back(std::make_unique<SSlotType<T>>(slot));
			break;

		case ESlotIo::Output:
			OutputSlots.push_back(std::make_unique<SSlotType<T>>(slot));
			break;
		}
	}
};

template<typename TNodeType>
TNodeType SNodeType<TNodeType>::StaticObject = {};

struct SNode final
{
	SNode(SNodeTypeBase& aNodeType)
		: NodeType(aNodeType) {}

	template<typename TOutputType>
	void SetOutput(i32 aIndex, TOutputType aOutputValue);

	template<typename TInputType>
	TInputType GetInput(i32 aIndex);

	template<typename TNode>
	SNodeType<TNode>& GetNodeType()
	{
		return SNodeType<TNode>::StaticObject;
	}

	SNodeTypeBase& NodeType;

	size_t Id = {};

	ImVec2 Position = {0.f, 0.f};

	bool Selected = false;

	bool IWantToDie = false;

	bool IsImmortal = false;

	std::unordered_map<std::string, std::any> NodeData;

	std::vector<std::unique_ptr<SInputSlotInstanceBase>> InputSlotInstances;
	std::vector<std::unique_ptr<SOutputSlotInstanceBase>> OutputSlotInstances;
};

template<typename TOutputType>
void SNode::SetOutput(i32 aIndex, TOutputType aOutputValue)
{
	SOutputSlotInstance<TOutputType>& output = static_cast<SOutputSlotInstance<TOutputType>&>(*OutputSlotInstances[aIndex]);

	output.OutputValue = aOutputValue;
}

template<typename TInputType>
TInputType SNode::GetInput(i32 aIndex)
{
	SInputSlotInstance<TInputType>& input = static_cast<SInputSlotInstance<TInputType>&>(*InputSlotInstances[aIndex]);

	if (!input.HasConnection())
		return input.EditableValue;

	SOutputSlotInstance<TInputType>& correspondingOutput = static_cast<SOutputSlotInstance<TInputType>&>(*input.Connection->OutputSlot);

	return correspondingOutput.OutputValue;
}
