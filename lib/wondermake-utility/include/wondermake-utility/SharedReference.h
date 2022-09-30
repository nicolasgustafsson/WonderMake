#pragma once

#include "wondermake-utility/Result.h"

#include <memory>

template<typename TType>
class SharedReference final
{
public:
	inline static [[nodiscard]] Result<SharedReference> FromPointer(std::shared_ptr<TType> aPointer) noexcept
	{
		if (!aPointer)
			return Err();

		return Ok(SharedReference(std::move(aPointer)));
	}

	SharedReference(const SharedReference&) noexcept = default;
	SharedReference(SharedReference&&) noexcept = default;
	template<typename TChild>
	inline SharedReference(const SharedReference<TChild>& aRhs) noexcept requires(std::is_constructible_v<std::shared_ptr<TType>, std::shared_ptr<TChild>>)
		: myPointer(aRhs)
	{}
	template<typename TChild>
	inline SharedReference(SharedReference<TChild>&& aRhs) noexcept requires(std::is_constructible_v<std::shared_ptr<TType>, std::shared_ptr<TChild>>)
		: myPointer(std::move(aRhs))
	{}

	SharedReference& operator=(const SharedReference&) noexcept = default;
	SharedReference& operator=(SharedReference &&) noexcept = default;
	template<typename TChild>
	SharedReference& operator=(const SharedReference<TChild>& aRhs) noexcept
	{
		myPointer = aRhs;

		return *this;
	}
	template<typename TChild>
	SharedReference& operator=(SharedReference<TChild>&& aRhs) noexcept
	{
		myPointer = std::move(aRhs);

		return *this;
	}

	bool operator==(const SharedReference&) const noexcept = default;

	inline [[nodiscard]] TType* operator->() noexcept
	{
		return myPointer.get();
	}
	inline [[nodiscard]] TType& operator*() noexcept
	{
		return *myPointer;
	}

	inline [[nodiscard]] const TType* operator->() const noexcept
	{
		return myPointer.get();
	}
	inline [[nodiscard]] const TType& operator*() const noexcept
	{
		return *myPointer;
	}

	inline [[nodiscard]] std::weak_ptr<TType> Weak() const noexcept
	{
		return myPointer;
	}

	inline [[nodiscard]] auto UseCount() const noexcept
	{
		return myPointer.use_count();
	}

	template<typename TToType>
	inline [[nodiscard]] operator std::shared_ptr<TToType>() const& noexcept requires(std::is_constructible_v<std::shared_ptr<TToType>, std::shared_ptr<TType>>)
	{
		return myPointer;
	}
	template<typename TToType>
	inline [[nodiscard]] operator std::shared_ptr<TToType>() && noexcept requires(std::is_constructible_v<std::shared_ptr<TToType>, std::shared_ptr<TType>>)
	{
		return std::move(myPointer);
	}
	
	template<typename TToType>
	inline [[nodiscard]] operator std::weak_ptr<TToType>() const& noexcept requires(std::is_constructible_v<std::weak_ptr<TToType>, std::shared_ptr<TType>>)
	{
		return myPointer;
	}
	template<typename TToType>
	inline [[nodiscard]] operator std::weak_ptr<TToType>() && noexcept requires(std::is_constructible_v<std::weak_ptr<TToType>, std::shared_ptr<TType>>)
	{
		return std::move(myPointer);
	}

	[[nodiscard]] inline constexpr size_t Hash() const noexcept
	{
		return std::hash<std::shared_ptr<TType>>()(myPointer);
	}

private:
	template<typename T>
	friend class SharedReference;

	inline SharedReference(std::shared_ptr<TType> aPointer) noexcept
		: myPointer(std::move(aPointer))
	{}

	std::shared_ptr<TType> myPointer;

};

namespace std
{
	template <typename TType>
	struct hash<SharedReference<TType>>
	{
		inline [[nodiscard]] size_t operator()(const SharedReference<TType>& aReference) const noexcept
		{
			return aReference.Hash();
		}
	};
}

template<typename TType, typename... TArgs>
inline [[nodiscard]] SharedReference<TType> MakeSharedReference(TArgs&&... aArgs)
{
	return SharedReference<TType>::FromPointer(std::make_shared<TType>(std::forward<TArgs>(aArgs)...)).Unwrap();
}

template<typename TTo, typename TFrom>
inline [[nodiscard]] SharedReference<TTo> StaticReferenceCast(SharedReference<TFrom> aReference)
{
	return SharedReference<TTo>::FromPointer(std::static_pointer_cast<TTo>(static_cast<std::shared_ptr<TFrom>>(std::move(aReference)))).Unwrap();
}
