#include "threadsafe-counter.h"

void work_func(ThreadSafeCounter& counter)
{
	for(int i = 0; i < 1000; ++i)
	{
		counter.increment();
	}
}

int main()
{
	ThreadSafeCounter counter;
	std::vector<std::thread> threads;
	for(int i = 0; i < 4; ++i)
	{
		threads.emplace_back(work_func, std::ref(counter));
	}
	for(auto& thread : threads)
	{
		thread.join();
	}
	std::cout << "Final count: " << counter.get_count() << std::endl;
	return 0;
}