#pragma once
#include <memory>
#include <vector>

struct SAttribute;

class Sketch
{
public:
	template<typename TAttribute>
	bool ContainsAttribute() const;

	template<typename TAttribute>
	void AddAttribute(TAttribute&& aAttribute);

	template<typename TAttribute>
	void RemoveAttribute();

	template<typename TAttribute>
	std::shared_ptr<TAttribute> GetAttribute() const;

	std::vector<std::shared_ptr<SAttribute>> myAttributes;
};

template<typename TAttribute>
bool Sketch::ContainsAttribute() const
{
	return GetAttribute<TAttribute>() != nullptr;
}

template<typename TAttribute>
void Sketch::AddAttribute(TAttribute&& aAttribute)
{
	myAttributes.emplace_back(std::make_shared<std::decay_t<TAttribute>>(std::forward<TAttribute>(aAttribute)));
}

template<typename TAttribute>
void Sketch::RemoveAttribute()
{
	auto it = std::find_if(myAttributes.begin(), myAttributes.end(), [](const auto& aAttribute) { return dynamic_cast<TAttribute*>(aAttribute.get()) != nullptr; });

	if (it == myAttributes.end())
		return;

	myAttributes.erase(it);
}

template<typename TAttribute>
std::shared_ptr<TAttribute> Sketch::GetAttribute() const
{
	auto it = std::find_if(myAttributes.begin(), myAttributes.end(), [](const auto& aAttribute) { return dynamic_cast<TAttribute*>(aAttribute.get()) != nullptr; });
	
	if (it == myAttributes.end())
		return nullptr;

	//bra kod
	return std::static_pointer_cast<TAttribute>(*it);
}

