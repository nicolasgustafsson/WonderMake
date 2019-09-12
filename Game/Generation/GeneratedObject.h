#pragma once

template <typename TGenerationParameter>
class GenerationOperations 
{
	GenerationOperations() 
	{
		static_assert(false, "You need to specialize GenerationOperations<T>!");
	}
};

template<typename ... TGenerationParameters>
class GeneratedObject 
	: public GenerationOperations<TGenerationParameters>...
{
public:
	virtual ~GeneratedObject() = default;

	virtual void Generate(TGenerationParameters... Args) = 0;
};

