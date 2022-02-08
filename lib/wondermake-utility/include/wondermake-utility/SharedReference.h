#pragma once

#include "wondermake-utility/Result.h"

#include <memory>

template<typename TType>
class SharedReference final
{
public:
	inline static [[nodiscard]] Result<decltype(Failure), SharedReference> FromPointer(std::shared_ptr<TType> aPointer) noexcept
	{
		if (!aPointer)
			return Failure;

		return SharedReference(std::move(aPointer));
	}

	SharedReference(const SharedReference&) noexcept = default;
	SharedReference(SharedReference&&) noexcept = default;

	SharedReference& operator=(const SharedReference&) noexcept = default;
	SharedReference& operator=(SharedReference &&) noexcept = default;

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

	template<typename TToType>
	inline [[nodiscard]] operator std::shared_ptr<TToType>() const noexcept requires(std::is_constructible_v<std::shared_ptr<TToType>, std::shared_ptr<TType>>)
	{
		return myPointer;
	}
	template<typename TParent>
	inline [[nodiscard]] operator SharedReference<TParent>() const noexcept requires(std::is_constructible_v<std::shared_ptr<TParent>, std::shared_ptr<TType>>)
	{
		return SharedReference<TParent>(myPointer);
	}

private:
	template<typename T>
	friend class SharedReference;

	inline SharedReference(std::shared_ptr<TType> aPointer) noexcept
		: myPointer(std::move(aPointer))
	{}

	std::shared_ptr<TType> myPointer;

};

template<typename TType, typename... TArgs>
inline [[nodiscard]] SharedReference<TType> MakeSharedReference(TArgs&&... aArgs)
{
	return SharedReference<TType>::FromPointer(std::make_shared<TType>(std::forward<TArgs>(aArgs)...));
}
