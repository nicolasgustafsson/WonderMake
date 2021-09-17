#pragma once

#include "Utilities/RestrictTypes.h"
#include "Utilities/Utility.h"

#include <type_traits>

#include "StaticRegistry/StaticRegistry.h"

namespace _Impl
{
	template<template<typename> typename TAllocator, typename TReturnValue, typename... TArgs>
	class _CallableImpl
		: NonCopyable
	{
	public:
		template<typename TCallable>
		static constexpr bool IsValidCallable = std::is_invocable_v<TCallable, TArgs...>
			&& !std::is_base_of_v<_CallableImpl<TAllocator, TReturnValue, TArgs...>, std::decay_t<TCallable>>
			&& std::is_nothrow_destructible_v<TCallable>;

		template<typename TCallable> requires IsValidCallable<TCallable>
		inline _CallableImpl(TCallable&& aCallable) noexcept
		{
			*this = std::forward<TCallable>(aCallable);
		}
		inline _CallableImpl(_CallableImpl&& aRhs) noexcept
		{
			*this = std::move(aRhs);
		}
		inline ~_CallableImpl() noexcept
		{
			Reset();
		}

		template<typename TCallable> requires IsValidCallable<TCallable>
		inline _CallableImpl& operator=(TCallable&& aCallable) noexcept
		{
			ConstructCallable<TCallable>(std::forward<TCallable>(aCallable));

			return *this;
		}
		inline _CallableImpl& operator=(_CallableImpl&& aRhs) noexcept
		{
			Reset();
			const auto moveTo = aRhs.GetMoveToFunc();

			std::swap(myPointers.GetOperationFunc, aRhs.myPointers.GetOperationFunc);
			if (moveTo)
				moveTo(aRhs, *this);
			else
				std::swap(myPointers.Data, aRhs.myPointers.Data);

			return *this;
		}

		inline TReturnValue operator()(TArgs... aArgs)
		{
			return GetInvokeFunc()(*this, std::forward<TArgs>(aArgs)...);
		}

	private:
		enum class EOperation
		{
			Invoke,
			Move,
			Destroy
		};

		struct Pointers
		{
			void*(*GetOperationFunc)(EOperation)	= nullptr;
			void* Data								= nullptr;
		};

		typedef TReturnValue(*OperationInvoke)(const _CallableImpl&, TArgs&&...);
		typedef void(*OperationMoveTo)(_CallableImpl&, _CallableImpl&);
		typedef void(*OperationDestroy)(_CallableImpl&);

		static constexpr std::size_t LocalStorageSize		= 64 - sizeof(Pointers);
		static constexpr std::size_t LocalStorageAlignment	= alignof(max_align_t);
		
		inline void Reset() noexcept
		{
			Utility::Invoke(GetDestroyFunc(), *this);
			
			myPointers.Data				= nullptr;
			myPointers.GetOperationFunc	= nullptr;
		}

		template<typename TCallable>
		inline void ConstructCallable(TCallable&& aCallable) noexcept
		{
			using Type = std::decay_t<TCallable>;

			constexpr auto fitsLocally = (sizeof(Type) <= LocalStorageSize) && (LocalStorageAlignment % std::alignment_of_v<Type> == 0);

			if constexpr (fitsLocally)
				myPointers.Data = static_cast<void*>(new(&myLocalStorage) Type(std::forward<TCallable>(aCallable)));
			else
				myPointers.Data = static_cast<void*>(TAllocator<Type>::Allocate(std::forward<TCallable>(aCallable)));
			myPointers.GetOperationFunc = &_CallableImpl::GetOperationFunc<TCallable>;
		}

		template<typename TCallable>
		static TReturnValue InvokeCallable(const _CallableImpl& aThis, TArgs&&... aArgs)
		{
			using Type = std::decay_t<TCallable>;

			Type* const data = static_cast<Type*>(aThis.myPointers.Data);

			return (*data)(std::forward<TArgs>(aArgs)...);
		}

		template<typename TCallable>
		static void MoveToCallableLocal(_CallableImpl& aThis, _CallableImpl& aTarget) noexcept
		{
			using Type = std::decay_t<TCallable>;

			Type* const data = static_cast<Type*>(aThis.myPointers.Data);

			aTarget.myPointers.Data = static_cast<void*>(new(&aTarget.myLocalStorage) Type(std::move(*data)));

			aThis.myPointers.Data = nullptr;
			data->~Type();
		}

		template<typename TCallable>
		static void DestroyCallableLocal(_CallableImpl& aThis) noexcept
		{
			using Type = std::decay_t<TCallable>;

			Type* const data = static_cast<Type*>(aThis.myPointers.Data);

			aThis.myPointers.Data = nullptr;
			data->~Type();
		}
		template<typename TCallable>
		static void DestroyCallableExternal(_CallableImpl& aThis) noexcept
		{
			using Type = std::decay_t<TCallable>;

			Type* const data = static_cast<Type*>(aThis.myPointers.Data);

			aThis.myPointers.Data = nullptr;
			TAllocator<Type>::Deallocate(data);
		}

		template<typename TCallable>
		static void* GetOperationFunc(const EOperation aOperation) noexcept
		{
			using Type = std::decay_t<TCallable>;

			constexpr auto fitsLocally = (sizeof(Type) <= LocalStorageSize) && (LocalStorageAlignment % std::alignment_of_v<Type> == 0);

			switch (aOperation)
			{
			case EOperation::Invoke:	return static_cast<void*>(&_CallableImpl::InvokeCallable<TCallable>);
			case EOperation::Move:		return fitsLocally ? static_cast<void*>(&_CallableImpl::MoveToCallableLocal<TCallable>) : nullptr;
			case EOperation::Destroy:	return fitsLocally ? static_cast<void*>(&_CallableImpl::DestroyCallableLocal<TCallable>) : static_cast<void*>(&_CallableImpl::DestroyCallableExternal<TCallable>);
			}

			return nullptr;
		}

		inline [[nodiscard]] OperationInvoke GetInvokeFunc() const noexcept
		{
			if (!myPointers.GetOperationFunc)
				return nullptr;

			return static_cast<OperationInvoke>(myPointers.GetOperationFunc(EOperation::Invoke));
		}
		inline [[nodiscard]] OperationMoveTo GetMoveToFunc() const noexcept
		{
			if (!myPointers.GetOperationFunc)
				return nullptr;

			return static_cast<OperationMoveTo>(myPointers.GetOperationFunc(EOperation::Move));
		}
		inline [[nodiscard]] OperationDestroy GetDestroyFunc() const noexcept
		{
			if (!myPointers.GetOperationFunc)
				return nullptr;

			return static_cast<OperationDestroy>(myPointers.GetOperationFunc(EOperation::Destroy));
		}

		Pointers myPointers;
		std::aligned_storage_t<LocalStorageSize, LocalStorageAlignment> myLocalStorage;

	};

	template<template<typename> typename TAllocator, typename T>
	struct _FunctionImpl
	{
		static_assert(AlwaysFalse<T>, "UniqueFunction must be function.");
	};

	template<template<typename> typename TAllocator, typename TReturnValue, typename... TArgs>
	struct _FunctionImpl<TAllocator, TReturnValue(TArgs...)>
	{
		using Callable = _CallableImpl<TAllocator, TReturnValue, TArgs...>;
	};
}

template<typename TType>
struct DefaultUniqueFunctionAllocator
{
	template<typename... TArgs>
	inline static [[nodiscard]] TType* Allocate(TArgs&&... aArgs)
	{
		return new TType(std::forward<TArgs>(aArgs)...);
	}
	inline static void Deallocate(TType* const aObject)
	{
		delete aObject;
	}
};

template<typename TFunction, template<typename> typename TAllocator = DefaultUniqueFunctionAllocator>
class UniqueFunction final
	: public _Impl::_FunctionImpl<TAllocator, TFunction>::Callable
{
private:
	using Super = typename _Impl::_FunctionImpl<TAllocator, TFunction>::Callable;

public:
	using Super::Super;

};

using Closure = UniqueFunction<void()>;

class ImmediateFunction
{
public:
	ImmediateFunction(Closure aFunction)
	{
		aFunction();
	}
};

template<void(TFunction)()>
class TImmediateFunction
{
public:
	TImmediateFunction()
	{
		TFunction();
	}
};

template<void(TFunction)()>
class StaticInitializationTimeFunction
{
public:
	StaticInitializationTimeFunction()
	{
		FORCE_INSTANTIATE(RunImmediately);
	}

	static inline TImmediateFunction<TFunction> RunImmediately;
};