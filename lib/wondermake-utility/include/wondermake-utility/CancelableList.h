#pragma once

#include "wondermake-utility/AnyExecutor.h"

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
	std::vector<RequestData>	myRequests;
	RequestIdType				myNextId = 0;

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
		auto onCancel = [&requestList = myRequests, requestId]()
		{
			const auto pred = [requestId](const auto& aRequest)
			{
				return aRequest.RequestId == requestId;
			};

			const auto it = std::find_if(requestList.begin(), requestList.end(), pred);

			if (it == requestList.end())
				return;

			auto cancelable = std::move(*it);

			cancelable;

			requestList.erase(it);
		};

		WmCallOnCancel(it->Cancelable, myExecutor, std::move(onCancel));

		return it;
	}

	inline iterator erase(const_iterator aWhere) noexcept
	{
		return myRequests.erase(aWhere.myIterator);
	}
	inline iterator erase(const_iterator aFirst, const_iterator aLast) noexcept
	{
		return myRequests.erase(aFirst.myIterator, aLast.myIterator);
	}

};
