#ifndef THREADSAFE_COUNTER_SUB_H
#define THREADSAFE_COUNTER_SUB_H

#include <iostream>
#include <list>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

class SubCounter
{
private:
	long long count_;
	mutable std::mutex mutex_; // mutable keyword allows const methods to lock the mutex
public:
	SubCounter()
		: count_(0LL)
	{ }
	~SubCounter() = default;

	long long increment()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return ++count_;
	}

	long long get_count() const
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return count_;
	}
};

/// @brief Thread-safe counter with sub-counters.
class ThreadSafeCounterWithSubCounter
{
private:
	std::list<SubCounter> sub_count_;
	mutable std::mutex mutex_; // mutable keyword allows const methods to lock the mutex

public:
	ThreadSafeCounterWithSubCounter() = default;

	ThreadSafeCounterWithSubCounter(const ThreadSafeCounterWithSubCounter&) = delete;
	ThreadSafeCounterWithSubCounter& operator=(const ThreadSafeCounterWithSubCounter&) = delete;
	ThreadSafeCounterWithSubCounter(ThreadSafeCounterWithSubCounter&&) = delete;
	ThreadSafeCounterWithSubCounter& operator=(ThreadSafeCounterWithSubCounter&&) = delete;
	~ThreadSafeCounterWithSubCounter() = default;

	std::list<SubCounter>::iterator register_counter()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		// Since SubCounter contains a mutex, which makes it non-copyable,
		// we need to use emplace to create a new SubCounter in place.
		return sub_count_.emplace(sub_count_.end());
	}

	long long increment(std::list<SubCounter>::iterator it)
	{
		return it->increment();
	}

	long long get_count()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto sum_sub_count = [](long long running_sum, const SubCounter& sub_counter) {
			return running_sum + sub_counter.get_count();
		};
		// actually, this accumulate will lock the mutex of each sub_counter one by one, but not lock all of them simultaneously
		// so if anyone calls increment() while we are accumulating, the result will be inconsistent
		long long sum = std::accumulate(sub_count_.begin(), sub_count_.end(), 0LL, sum_sub_count);
		return sum;
	}
};

#endif // THREADSAFE_COUNTER_SUB_H