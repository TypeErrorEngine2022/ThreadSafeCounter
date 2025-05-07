#ifndef NON_THREADSAFE_COUNTER_H
#define NON_THREADSAFE_COUNTER_H

#include <iostream>
#include <vector>

/// @brief Non-Thread-safe counter implementation
class NonThreadSafeCounter
{
public:
	NonThreadSafeCounter()
		: count_(0)
	{ }

	NonThreadSafeCounter(const NonThreadSafeCounter&) = delete;
	NonThreadSafeCounter& operator=(const NonThreadSafeCounter&) = delete;
	NonThreadSafeCounter(NonThreadSafeCounter&&) = delete;
	NonThreadSafeCounter& operator=(NonThreadSafeCounter&&) = delete;
	~NonThreadSafeCounter() = default;

	void increment()
	{
		++count_;
	}

	long long get_count() const
	{
		return count_;
	}

private:
	long long count_;
};

#endif // NON_THREADSAFE_COUNTER_H