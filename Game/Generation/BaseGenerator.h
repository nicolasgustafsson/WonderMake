#pragma once
#include "System/System.h"
#include <type_traits>
#include "Utilities/Debugging/Debugged.h"


class BaseGenerator
	: public System
{
public:
	BaseGenerator() = default;
	virtual ~BaseGenerator() = default;
};

template <typename TGeneratedType, typename TGenerationParameters>
class Grammar
{
public:
	virtual f32 GetPriority(TGeneratedType& aObject, const TGenerationParameters& aParameters) const = 0;
	virtual void Modify(TGeneratedType& aObject, TGenerationParameters& aParameters) const = 0;
};

template<typename TGeneratedType, typename TGenerationParameters>
class Generator 
	: public BaseGenerator
	, public Debugged
{
public:

	virtual TGeneratedType Generate(TGenerationParameters& aGenerationParameters);

protected:
	template<typename TGrammar>
	void AddGrammar();

	using TBaseGrammarType = std::unique_ptr<Grammar<TGeneratedType, TGenerationParameters>>;
private:
	virtual bool ShouldApplyGrammars(const TGeneratedType& aObject, const TGenerationParameters& aGenerationParameters) const = 0;
	virtual void ImplementGrammars() = 0;

	virtual void PreProcess(TGeneratedType& aObject) {};

	std::vector<TBaseGrammarType> myGrammars;

	virtual void Debug() override;
};

template<typename TGeneratedType, typename TGenerationParameters>
void Generator<TGeneratedType, TGenerationParameters>::Debug()
{
	std::string name = typeid(TGeneratedType).name();
	name += " Generator";
	ImGui::Begin(name.c_str());

	TGenerationParameters parameters;

	static std::unique_ptr<TGeneratedType> generated;

	if (ImGui::Button("Generate!"))
	{
		generated = std::make_unique<TGeneratedType>(Generate(parameters));
	}

	//if constexpr (std::is_invocable<decltype(&TGeneratedType::Inspect)>::value)
	//{
	//	if (generated)
	//		generated->Inspect();
	//}

	ImGui::End();
}

template<typename TGeneratedType, typename TGenerationParameters>
TGeneratedType Generator<TGeneratedType, TGenerationParameters>::Generate(TGenerationParameters& aGenerationParameters)
{
	TGeneratedType generatedObject = TGeneratedType();

	ImplementGrammars();

	PreProcess(generatedObject);

	if (myGrammars.size() == 0)
		return generatedObject;

	while (ShouldApplyGrammars(generatedObject, aGenerationParameters))
	{
		std::sort(myGrammars.begin(), myGrammars.end(), [&](const TBaseGrammarType& aLeftGrammar,
			const TBaseGrammarType& aRightGrammar)
			{
				return aLeftGrammar->GetPriority(generatedObject, aGenerationParameters) < aRightGrammar->GetPriority(generatedObject, aGenerationParameters);
			});

		myGrammars[0]->Modify(generatedObject, aGenerationParameters);
	}

	return generatedObject;
}

template<typename TGeneratedType, typename TGenerationParameters>
template<typename TGrammar>
void Generator<TGeneratedType, TGenerationParameters>::AddGrammar()
{
	static_assert(std::is_default_constructible_v<TGrammar>, "Grammars must be default constructible!");
	myGrammars.push_back(std::make_unique<TGrammar>());
}
