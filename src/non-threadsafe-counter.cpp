#include "headers/non-threadsafe-counter.h"
#include <thread>

void work_func(NonThreadSafeCounter& counter)
{
	for(int i = 0; i < 10000; ++i)
	{
		counter.increment();
	}
}

int main()
{
	NonThreadSafeCounter counter;
	std::vector<std::thread> threads;
	for(int i = 0; i < 4; ++i)
	{
		threads.emplace_back(work_func, std::ref(counter));
	}
	for(auto& thread : threads)
	{
		thread.join();
	}
	std::cout << "Final count: " << counter.get_count() << std::endl; // will this be 4 * 10000 = 40000?
	return 0;
}