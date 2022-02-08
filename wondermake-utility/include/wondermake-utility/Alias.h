#pragma once

#include "Utility.h"

#include <type_traits>

template<typename TType>
class Alias
{
public:
	template<typename TAlias>
	static constexpr bool IsValidAlias = (sizeof(TType) == sizeof(TAlias))
		&& (std::alignment_of_v<TType> % std::alignment_of_v<TAlias> == 0)
		&& (!std::is_copy_assignable_v<TType> || std::is_copy_assignable_v<TAlias>)
		&& (!std::is_move_assignable_v<TType> || std::is_move_assignable_v<TAlias>)
		&& (!std::is_nothrow_copy_assignable_v<TType> || std::is_nothrow_copy_assignable_v<TAlias>)
		&& (!std::is_nothrow_move_assignable_v<TType> || std::is_nothrow_move_assignable_v<TAlias>)
		&& (!std::is_nothrow_destructible_v<TType> || std::is_nothrow_destructible_v<TAlias>)
		&& (std::is_copy_assignable_v<TAlias> || std::is_move_assignable_v<TAlias>);
	
	inline Alias() noexcept(std::is_nothrow_default_constructible_v<TType>) requires std::is_default_constructible_v<TType>
	{
		Construct<TType>();
	}
	template<typename TAlias> requires IsValidAlias<TAlias>
	inline Alias(TAlias&& aAlias) noexcept(std::is_nothrow_constructible_v<decltype(std::forward<TAlias>(aAlias))>)
	{
		Construct<TAlias>(std::forward<TAlias>(aAlias));
	}
	inline Alias(const Alias& aRhs) noexcept(std::is_nothrow_copy_assignable_v<TType>) requires std::is_copy_assignable_v<TType>
	{
		aRhs.InvokeOpCopy(*this);
	}
	inline Alias(Alias&& aRhs) noexcept(std::is_nothrow_move_assignable_v<TType>) requires std::is_move_assignable_v<TType>
	{
		aRhs.InvokeOpMove(*this);
	}
	inline ~Alias() noexcept(std::is_nothrow_destructible_v<TType>)
	{
		InvokeOpDestroy();
	}
	
	template<typename TAlias> requires IsValidAlias<TAlias>
	inline Alias& operator=(TAlias&& aRhs) noexcept(std::is_nothrow_constructible_v<decltype(std::forward<TAlias>(aRhs))>)
	{
		Construct<TAlias>(std::forward<TAlias>(aRhs));

		return *this;
	}
	inline Alias& operator=(const Alias& aRhs) noexcept(std::is_nothrow_copy_assignable_v<TType>) requires std::is_copy_assignable_v<TType>
	{
		aRhs.InvokeOpCopy(*this);

		return *this;
	}
	inline Alias& operator=(Alias&& aRhs) noexcept(std::is_nothrow_move_assignable_v<TType>) requires std::is_move_assignable_v<TType>
	{
		aRhs.InvokeOpMove(*this);

		return *this;
	}
	
	template<typename TAlias, typename... TArgs> requires IsValidAlias<TAlias>
	inline TAlias& Emplace(TArgs&&... aArgs) noexcept(std::is_nothrow_constructible_v<TAlias, decltype(std::forward<TArgs>(aArgs))...>)
	{
		return Construct<TAlias>(std::forward<TArgs>(aArgs)...);
	}

private:
	typedef void(*OpPtrCopy)(const Alias&, Alias&);
	typedef void(*OpPtrMove)(Alias&&, Alias&);
	typedef void(*OpPtrDestroy)(Alias&);

	enum class EOperation
	{
		Copy,
		Move,
		Destroy
	};

	template<typename TAlias>
	static void OpCopy(const Alias& aSource, Alias& aTarget) noexcept(std::is_nothrow_copy_assignable_v<TType>)
	{
		const auto* const data = reinterpret_cast<const TAlias*>(&aSource.myData);

		new(&aTarget.myData) TAlias(*data);
		aTarget.myOperationFunc = aSource.myOperationFunc;
	}
	template<typename TAlias>
	static void OpMove(Alias&& aSource, Alias& aTarget) noexcept(std::is_nothrow_move_assignable_v<TType>)
	{
		auto* const data = reinterpret_cast<TAlias*>(&aSource.myData);

		new(&aTarget.myData) TAlias(std::move(*data));
		aTarget.myOperationFunc = aSource.myOperationFunc;

		data->~TAlias();
		aSource.myOperationFunc = nullptr;
	}
	template<typename TAlias>
	static void OpDestroy(Alias& aThis) noexcept(std::is_nothrow_destructible_v<TType>)
	{
		auto* const data = reinterpret_cast<TAlias*>(&aThis.myData);

		data->~TAlias();
		aThis.myOperationFunc = nullptr;
	}

	template<typename TAlias>
	static void* GetOperation(const EOperation aOperation) noexcept
	{
		switch (aOperation)
		{
		case EOperation::Move:
			if constexpr (std::is_move_assignable_v<TType>)
				return static_cast<void*>(&Alias::OpMove<TAlias>);
		case EOperation::Copy:
			if constexpr (std::is_copy_assignable_v<TType>)
				return static_cast<void*>(&Alias::OpCopy<TAlias>);
			else
				break;
		case EOperation::Destroy:
			return static_cast<void*>(&Alias::OpDestroy<TAlias>);
		}

		return nullptr;
	}

	inline void InvokeOpCopy(Alias& aTarget) noexcept(std::is_nothrow_copy_assignable_v<TType>)
	{
		aTarget.InvokeOpDestroy();

		if (myOperationFunc)
			Utility::Invoke(static_cast<OpPtrCopy>(myOperationFunc(EOperation::Copy)), *this, aTarget);
	}
	inline void InvokeOpMove(Alias&& aTarget) noexcept(std::is_nothrow_move_assignable_v<TType>)
	{
		aTarget.InvokeOpDestroy();

		if (myOperationFunc)
			Utility::Invoke(static_cast<OpPtrMove>(myOperationFunc(EOperation::Move)), *this, std::move(aTarget));
	}
	inline void InvokeOpDestroy() noexcept(std::is_nothrow_destructible_v<TType>)
	{
		if (myOperationFunc)
			Utility::Invoke(static_cast<OpPtrDestroy>(myOperationFunc(EOperation::Destroy)), *this);
	}

	template<typename TAlias, typename... TArgs>
	inline TAlias& Construct(TArgs&&... aArgs) noexcept(std::is_nothrow_constructible_v<TAlias, decltype(std::forward<TArgs>(aArgs))...>)
	{
		InvokeOpDestroy();

		TAlias* const data = new(&myData) TAlias(std::forward<TArgs>(aArgs)...);
		myOperationFunc = &Alias::GetOperation<TAlias>;

		return *data;
	}

	std::aligned_storage_t<sizeof(TType), std::alignment_of_v<TType>> myData;
	void*(*myOperationFunc)(const EOperation) = nullptr;

};
