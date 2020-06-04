#pragma once

#include <thread>

class ThreadChecker
{
public:

	inline ThreadChecker();

	inline void AttachToCurrentThread() noexcept;
	inline bool IsCurrentThread() const noexcept;
	inline void RequireThread() const noexcept;

private:

	std::thread::id myId;

};

ThreadChecker::ThreadChecker()
{
	AttachToCurrentThread();
}

inline void ThreadChecker::AttachToCurrentThread() noexcept
{
	myId = std::this_thread::get_id();
}
inline bool ThreadChecker::IsCurrentThread() const noexcept
{
	return myId == std::this_thread::get_id();
}
inline void ThreadChecker::RequireThread() const noexcept
{
	assert(IsCurrentThread() && "Thread is not the required thread.");
}
