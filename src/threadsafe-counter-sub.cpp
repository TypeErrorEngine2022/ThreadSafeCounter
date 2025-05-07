#include "headers/threadsafe-counter-sub.h"

void work_func(ThreadSafeCounterWithSubCounter& counter)
{
	auto it = counter.register_counter();
	for(int i = 0; i < 1000; ++i)
	{
		counter.increment(it);
	}
}

int main()
{
	ThreadSafeCounterWithSubCounter counter;
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