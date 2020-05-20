#pragma once
#include "NodeGraph/SlotColors.h"
#include "NodeGraph/InputSlotEdits.h"

struct SSlotTypeBase;

struct SConnection
{
	void* InputNodeId = nullptr;
	void* OutputNodeId = nullptr;
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
};


struct SSlotTypeBase abstract
{
	std::string Name = "";

	virtual ImColor GetColor() const = 0;
	virtual size_t GetTypeHash() const = 0;

	virtual std::unique_ptr<SInputSlotInstanceBase> CreateInputSlotInstance() const = 0;
	virtual std::unique_ptr<SOutputSlotInstanceBase> CreateOutputSlotInstance() const = 0;
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
		: Title(aTitle)
	{

	}
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
};

template<typename TNodeType>
struct NodeType : public SNodeTypeBase
{
	NodeType(std::string aTitle)
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
TNodeType NodeType<TNodeType>::StaticObject = {};

struct SNode
{
	SNode(SNodeTypeBase& aNodeType)
		: NodeType(aNodeType)
	{

	}

	SNodeTypeBase& NodeType;

	size_t Id;

	ImVec2 Position;

	bool Selected;

	bool IWantToDie = false;

	bool IsImmortal = false;

	std::vector<std::unique_ptr<SInputSlotInstanceBase>> InputSlotInstances;
	std::vector<std::unique_ptr<SOutputSlotInstanceBase>> OutputSlotInstances;
};
