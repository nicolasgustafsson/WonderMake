#pragma once

#include "wondermake-utility/AnyExecutor.h"

#include <list>
#include <vector>

template<typename TCancelable>
concept CCancelableEmbedded = requires(TCancelable& aCancelable, AnyExecutor aExecutor, Closure aOnCancel)
{
	aCancelable.OnCancel(std::move(aExecutor), std::move(aOnCancel));
};

template<typename TCancelable>
concept CCancelableNonEmbedded = requires(TCancelable& aCancelable, AnyExecutor aExecutor, Closure aOnCancel)
{
	WmOnCancel(aCancelable, std::move(aExecutor), std::move(aOnCancel));
};

template<typename TCancelable>
concept CCancelable = CCancelableEmbedded<TCancelable> || CCancelableNonEmbedded<TCancelable>;

template<CCancelable TCancelable>
inline void WmCallOnCancel(TCancelable& aCancelable, CExecutor auto&& aExecutor, auto&& aOnCancel)
{
	if constexpr (CCancelableEmbedded<TCancelable>)
		aCancelable.OnCancel(std::forward<decltype(aExecutor)>(aExecutor), std::forward<decltype(aOnCancel)>(aOnCancel));
	else if constexpr (CCancelableNonEmbedded<TCancelable>)
		WmOnCancel(aCancelable, std::forward<decltype(aExecutor)>(aExecutor), std::forward<decltype(aOnCancel)>(aOnCancel));
}

template<CCancelable TCancelable>
class CancelableList
{
private:
	// Having the RequestId's type be that of a pointer guarantees that
	// the application runs out of memory before it runs out of ids.
	using RequestIdType = std::uintptr_t;

	struct RequestData
	{
		TCancelable		Cancelable;
		RequestIdType	RequestId;
	};

	AnyExecutor					myExecutor;
	std::list<RequestData>		myRequests;
	std::vector<RequestIdType>	myEraseBuffer;
	RequestIdType				myNextId = 0;
	bool						myIsErasing = false;

public:
	template<typename TIterator, bool TIsConst>
	class Iterator
	{
	public:
		friend CancelableList;
		template<typename TIterator, bool TIsConst>
		friend class Iterator;

		using iterator_category	= std::forward_iterator_tag;
		using difference_type	= std::ptrdiff_t;
		using value_type		= TCancelable;
		using pointer			= std::conditional_t<TIsConst, const TCancelable*, TCancelable*>;
		using reference			= std::conditional_t<TIsConst, const TCancelable&, TCancelable&>;

		inline Iterator(TIterator aIterator) noexcept
			: myIterator(std::move(aIterator))
		{}
		inline Iterator(const Iterator&) noexcept = default;
		template<typename TRhsIterator>
		inline Iterator(const Iterator<TRhsIterator, false>& aRhs) noexcept
			requires(TIsConst)
			: myIterator(aRhs.myIterator)
		{}

		inline Iterator& operator=(const Iterator&) noexcept = default;
		template<typename TRhsIterator>
		inline Iterator& operator=(const Iterator<TRhsIterator, false>& aRhs) noexcept
			requires(TIsConst)
			: myIterator(aRhs.myIterator)
		{}

		inline auto operator<=>(const Iterator&) const noexcept = default;

		inline Iterator& operator++() noexcept
		{
			++myIterator;

			return *this;
		}
		inline [[nodiscard]] Iterator operator++(int) noexcept
		{
			auto retVal = myIterator;

			++myIterator;

			return retVal;
		}

		inline [[nodiscard]] Iterator& operator+=(difference_type aOffset) noexcept
		{
			myIterator += aOffset;

			return *this;
		}
		inline [[nodiscard]] Iterator operator+(difference_type aOffset) const noexcept
		{
			return myIterator + aOffset;
		}

		inline [[nodiscard]] reference operator*() const noexcept
		{
			return myIterator->Cancelable;
		}
		inline [[nodiscard]] pointer operator->() const noexcept
		{
			return &myIterator->Cancelable;
		}

	private:
		TIterator myIterator;

	};

	using iterator = Iterator<typename decltype(myRequests)::iterator, false>;
	using const_iterator = Iterator<typename decltype(myRequests)::const_iterator, true>;

	inline CancelableList(AnyExecutor aExecutor) noexcept
		: myExecutor(std::move(aExecutor))
	{}

	inline void Emplace(TCancelable aCancelable)
	{
		(void)insert(cend(), std::move(aCancelable));
	}
	
	inline [[nodiscard]] TCancelable Pop() noexcept
	{
		auto requestData = std::move(myRequests.front());

		myRequests.erase(myRequests.begin());

		return std::move(requestData.Cancelable);
	}
	inline [[nodiscard]] void Clear() noexcept
	{
		return myRequests.clear();
	}
	inline [[nodiscard]] bool IsEmpty() const noexcept
	{
		return myRequests.empty();
	}

	inline [[nodiscard]] size_t size() const noexcept
	{
		return myRequests.size();
	}

	inline [[nodiscard]] iterator begin() noexcept
	{
		return myRequests.begin();
	}
	inline [[nodiscard]] iterator end() noexcept
	{
		return myRequests.end();
	}
	inline [[nodiscard]] const_iterator begin() const noexcept
	{
		return myRequests.begin();
	}
	inline [[nodiscard]] const_iterator end() const noexcept
	{
		return myRequests.end();
	}
	inline [[nodiscard]] const_iterator cbegin() const noexcept
	{
		return myRequests.cbegin();
	}
	inline [[nodiscard]] const_iterator cend() const noexcept
	{
		return myRequests.cend();
	}

	inline iterator insert(const_iterator aWhere, const TCancelable& aCancelable) noexcept
	{
		return insert(aWhere, TCancelable(aCancelable));
	}
	inline iterator insert(const_iterator aWhere, TCancelable&& aCancelable) noexcept
	{
		const auto getNextRequestId = [&requestList = myRequests, &nextId = myNextId]() -> RequestIdType
		{
			for (RequestIdType requestId = nextId;; ++requestId)
			{
				const auto pred = [requestId](const auto& aRequest)
				{
					return aRequest.RequestId == requestId;
				};

				if (std::find_if(requestList.begin(), requestList.end(), pred) == requestList.end())
				{
					nextId = requestId + 1;

					return requestId;
				}
			}
		};

		const auto requestId = getNextRequestId();

		const auto it = myRequests.insert(aWhere.myIterator, RequestData{ std::move(aCancelable), requestId });
		auto onCancel = [this, requestId]()
		{
			EraseCallback(requestId);
		};

		WmCallOnCancel(it->Cancelable, myExecutor, std::move(onCancel));

		return it;
	}

	inline iterator erase(const_iterator aWhere) noexcept
	{
		const auto findIt = [this, &aWhere]()
		{
			for (auto it = myRequests.begin(); it != myRequests.end(); ++it)
				if (it == aWhere.myIterator)
					return it;

			return myRequests.end();
		};

		auto it = findIt();

		bool alreadyErasing = myIsErasing;

		myIsErasing = true;

		auto nextIt = EraseInternal(it);

		auto eraseBuffer = std::exchange(myEraseBuffer, decltype(myEraseBuffer)());

		for (size_t i = 0; i < eraseBuffer.size(); ++i)
		{
			const auto pred = [id = eraseBuffer[i]](const auto& aRequest)
			{
				return aRequest.RequestId == id;
			};

			const auto eraseIt = std::find_if(myRequests.begin(), myRequests.end(), pred);

			if (eraseIt == myRequests.end())
				continue;

			auto nextEraseIt = EraseInternal(eraseIt);

			if (eraseIt == nextIt)
				nextIt = nextEraseIt;
		}

		if (!alreadyErasing)
			myIsErasing = false;

		return nextIt;
	}
	inline iterator erase(const_iterator aFirst, const_iterator aLast) noexcept
	{
		const auto findIt = [this](const_iterator aWhere)
		{
			for (auto it = myRequests.begin(); it != myRequests.end(); ++it)
				if (it == aWhere.myIterator)
					return it;

			return myRequests.end();
		};

		if (aFirst == aLast)
			return findIt(aLast);

		auto itFirst	= findIt(aFirst);
		auto itLast		= findIt(aLast);

		bool alreadyErasing = myIsErasing;

		myIsErasing = true;

		auto eraseSpan = [this](iterator aFirst, iterator aLast) -> iterator
		{
			std::vector<TCancelable> removedList;

			removedList.reserve(std::distance(aFirst, aLast));

			for (auto it = aFirst; it != aLast; ++it)
				removedList.emplace_back(std::move(*it));

			return myRequests.erase(aFirst.myIterator, aLast.myIterator);
		};

		iterator nextIt = eraseSpan(itFirst, itLast);

		auto eraseBuffer = std::exchange(myEraseBuffer, decltype(myEraseBuffer)());

		for (size_t i = 0; i < eraseBuffer.size(); ++i)
		{
			const auto pred = [id = eraseBuffer[i]](const auto& aRequest)
			{
				return aRequest.RequestId == id;
			};

			const auto eraseIt = std::find_if(myRequests.begin(), myRequests.end(), pred);

			if (eraseIt == myRequests.end())
				continue;

			auto nextEraseIt = EraseInternal(eraseIt);

			if (eraseIt == nextIt)
				nextIt = nextEraseIt;
		}

		if (!alreadyErasing)
			myIsErasing = false;

		return nextIt;
	}

private:
	inline void EraseCallback(RequestIdType aId) noexcept
	{
		if (myIsErasing)
		{
			myEraseBuffer.emplace_back(aId);

			return;
		}

		const auto pred = [aId](const auto& aRequest)
		{
			return aRequest.RequestId == aId;
		};

		const auto it = std::find_if(myRequests.begin(), myRequests.end(), pred);

		if (it == myRequests.end())
			return;

		EraseInternal(it);
	}
	inline iterator EraseInternal(iterator aWhere) noexcept
	{
		auto temp = std::move(*aWhere);

		auto nextIt = myRequests.erase(aWhere.myIterator);

		// This is never used, it's just to make sure that its destructor is called after erase
		temp;

		return nextIt;
	}


};
