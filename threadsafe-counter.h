#ifndef THREADSAFE_COUNTER_H
#define THREADSAFE_COUNTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

/// @brief Thread-safe counter implementation using std::mutex
class ThreadSafeCounter
{
public:
	ThreadSafeCounter()
		: count_(0)
	{ }

	ThreadSafeCounter(const ThreadSafeCounter&) = delete;
	ThreadSafeCounter& operator=(const ThreadSafeCounter&) = delete;
	ThreadSafeCounter(ThreadSafeCounter&&) = delete;
	ThreadSafeCounter& operator=(ThreadSafeCounter&&) = delete;
	~ThreadSafeCounter() = default;

	void increment()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		++count_;
	}

	long long get_count() const
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return count_;
	}

private:
	long long count_;
	mutable std::mutex mutex_; // mutable keyword allows const methods to lock the mutex
};

#endif // THREADSAFE_COUNTER_H