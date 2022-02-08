#pragma once
template<typename SingletonType>
class Singleton
{
public:
	static SingletonType& Get();

protected:
	Singleton() = default;
};

template<typename SingletonType>
SingletonType& Singleton<SingletonType>::Get()
{
	static SingletonType Self;

	return Self;
}

