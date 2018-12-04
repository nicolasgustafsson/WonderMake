#pragma once
template<typename SingletonType>
class Singleton
{
public:
	static SingletonType* Get();

protected:
	Singleton() = default;

	static std::unique_ptr<SingletonType> Self;
};

template<typename SingletonType>
std::unique_ptr<SingletonType> Singleton<SingletonType>::Self = nullptr;

template<typename SingletonType>
SingletonType* Singleton<SingletonType>::Get()
{
	if (!Self)
		Self = std::make_unique<SingletonType>();

	return Self.get();
}

