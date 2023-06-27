#pragma once

#include "wondermake-utility/SharedReference.h"

#include <cassert>
#include <memory>

template<typename TResource>
class FileResourcePtr;
template<typename TResource>
class FileResourceWPtr;

template<typename TResource>
class FileResourceRef final
{
public:
	friend FileResourcePtr<TResource>;

	FileResourceRef(const FileResourceRef&) noexcept = default;
	FileResourceRef(FileResourceRef&&) noexcept = default;

	FileResourceRef& operator=(const FileResourceRef&) noexcept = default;
	FileResourceRef& operator=(FileResourceRef&&) noexcept = default;
	
	[[nodiscard]] inline bool operator==(const FileResourceRef& aRhs) const noexcept
	{
		return myDoublePointer == aRhs.myDoublePointer;
	}
	[[nodiscard]] inline bool operator!=(const FileResourceRef& aRhs) const noexcept
	{
		return myDoublePointer != aRhs.myDoublePointer;
	}
	
	[[nodiscard]] inline FileResourceWPtr<TResource> Weak() noexcept
	{
		return FileResourceWPtr<TResource>(*this);
	}

	[[nodiscard]] inline TResource* operator->() const noexcept
	{
		return &**myDoublePointer;
	}
	[[nodiscard]] inline TResource& operator*() const noexcept
	{
		return **myDoublePointer;
	}

	[[nodiscard]] inline constexpr size_t Hash() const noexcept
	{
		return std::hash<std::shared_ptr<SharedReference<TResource>>>()(myDoublePointer);
	}

private:
	FileResourceRef(std::shared_ptr<SharedReference<TResource>> aPtr) noexcept
		: myDoublePointer(std::move(aPtr))
	{};

	std::shared_ptr<SharedReference<TResource>> myDoublePointer;

};

template<typename TResource>
class FileResourcePtr final
{
public:
	FileResourcePtr() noexcept = default;
	FileResourcePtr(std::shared_ptr<SharedReference<TResource>> aPtr) noexcept
		: myDoublePointer(std::move(aPtr))
	{};
	FileResourcePtr(std::nullptr_t) noexcept {};
	FileResourcePtr(const FileResourcePtr&) noexcept = default;
	FileResourcePtr(FileResourcePtr&&) noexcept = default;
	FileResourcePtr(const FileResourceRef<TResource>& aRhs) noexcept
		: myDoublePointer(aRhs.myDoublePointer)
	{};
	FileResourcePtr(FileResourceRef<TResource>&& aRhs) noexcept
		: myDoublePointer(std::move(aRhs.myDoublePointer))
	{};

	FileResourcePtr& operator=(const FileResourcePtr&) noexcept = default;
	FileResourcePtr& operator=(FileResourcePtr&&) noexcept = default;
	FileResourcePtr& operator=(FileResourceRef<TResource> aRhs) noexcept
	{
		myDoublePointer = std::move(aRhs.myDoublePointer);

		return *this;
	}
	
	[[nodiscard]] inline bool operator==(const FileResourcePtr& aRhs) const noexcept
	{
		return myDoublePointer == aRhs.myDoublePointer;
	}
	[[nodiscard]] inline bool operator!=(const FileResourcePtr& aRhs) const noexcept
	{
		return myDoublePointer != aRhs.myDoublePointer;
	}
	[[nodiscard]] inline bool operator==(const FileResourceRef<TResource>& aRhs) const noexcept
	{
		return myDoublePointer == aRhs.myDoublePointer;
	}
	[[nodiscard]] inline bool operator!=(const FileResourceRef<TResource>& aRhs) const noexcept
	{
		return myDoublePointer != aRhs.myDoublePointer;
	}

	[[nodiscard]] inline bool IsValid() const noexcept
	{
		return static_cast<bool>(myDoublePointer);
	}
	[[nodiscard]] inline operator bool() const noexcept
	{
		return IsValid();
	}

	[[nodiscard]] inline FileResourceRef<TResource> ToRef() const& noexcept
	{
		assert(myDoublePointer && "Dereferenced nullptr");

		return FileResourceRef<TResource>(myDoublePointer);
	}
	[[nodiscard]] inline FileResourceRef<TResource> ToRef() && noexcept
	{
		assert(myDoublePointer && "Dereferenced nullptr");

		return FileResourceRef<TResource>(std::move(myDoublePointer));
	}

	[[nodiscard]] inline FileResourceWPtr<TResource> Weak() noexcept
	{
		return FileResourceWPtr<TResource>(*this);
	}

	[[nodiscard]] inline TResource* operator->() const noexcept
	{
		return &**myDoublePointer;
	}
	[[nodiscard]] inline TResource& operator*() const noexcept
	{
		return **myDoublePointer;
	}

	[[nodiscard]] inline constexpr size_t Hash() const noexcept
	{
		return std::hash<std::shared_ptr<SharedReference<TResource>>>()(myDoublePointer);
	}

private:
	std::shared_ptr<SharedReference<TResource>> myDoublePointer;

};

template<typename TResource>
class FileResourceWPtr final
{
public:
	FileResourceWPtr() noexcept = default;
	FileResourceWPtr(const FileResourceWPtr&) noexcept = default;
	FileResourceWPtr(FileResourceWPtr&&) noexcept = default;
	FileResourceWPtr(const FileResourceRef<TResource>& aRhs) noexcept
		: myDoublePointer(aRhs.myDoublePointer)
	{};
	FileResourceWPtr(FileResourceRef<TResource>&& aRhs) noexcept
		: myDoublePointer(std::move(aRhs.myDoublePointer))
	{};
	FileResourceWPtr(const FileResourcePtr<TResource>& aRhs) noexcept
		: myDoublePointer(aRhs.myDoublePointer)
	{};
	FileResourceWPtr(FileResourcePtr<TResource>&& aRhs) noexcept
		: myDoublePointer(std::move(aRhs.myDoublePointer))
	{};

	FileResourceWPtr& operator=(const FileResourceWPtr&) noexcept = default;
	FileResourceWPtr& operator=(FileResourceWPtr&&) noexcept = default;
	FileResourceWPtr& operator=(const FileResourceRef<TResource>& aRhs) noexcept
	{
		myDoublePointer = aRhs.myDoublePointer;

		return *this;
	}
	FileResourceWPtr& operator=(const FileResourcePtr<TResource>& aRhs) noexcept
	{
		myDoublePointer = aRhs.myDoublePointer;

		return *this;
	}
	
	[[nodiscard]] inline bool operator==(const FileResourceWPtr& aRhs) const noexcept
	{
		return myDoublePointer == aRhs.myDoublePointer;
	}
	[[nodiscard]] inline bool operator!=(const FileResourceWPtr& aRhs) const noexcept
	{
		return myDoublePointer != aRhs.myDoublePointer;
	}
	[[nodiscard]] inline bool operator==(const FileResourceRef<TResource>& aRhs) const noexcept
	{
		return myDoublePointer == aRhs.myDoublePointer;
	}
	[[nodiscard]] inline bool operator!=(const FileResourceRef<TResource>& aRhs) const noexcept
	{
		return myDoublePointer != aRhs.myDoublePointer;
	}

	[[nodiscard]] inline bool IsValid() const noexcept
	{
		return static_cast<bool>(myDoublePointer);
	}
	[[nodiscard]] inline FileResourcePtr<TResource> Lock() const noexcept
	{
		return FileResourcePtr<TResource>(myDoublePointer.lock());
	}

	[[nodiscard]] inline constexpr size_t Hash() const noexcept
	{
		return std::hash<std::weak_ptr<SharedReference<TResource>>>()(myDoublePointer);
	}

private:
	std::weak_ptr<SharedReference<TResource>> myDoublePointer;

};

namespace std
{
	template<typename TResource>
	struct hash<FileResourceRef<TResource>>
	{
		inline [[nodiscard]] size_t operator()(const FileResourceRef<TResource>& aReference) const noexcept
		{
			return aReference.Hash();
		}
	};

	template<typename TResource>
	struct hash<FileResourcePtr<TResource>>
	{
		inline [[nodiscard]] size_t operator()(const FileResourcePtr<TResource>& aPointer) const noexcept
		{
			return aPointer.Hash();
		}
	};

	template<typename TResource>
	struct hash<FileResourceWPtr<TResource>>
	{
		inline [[nodiscard]] size_t operator()(const FileResourceWPtr<TResource>& aPointer) const noexcept
		{
			return aPointer.Hash();
		}
	};
}
