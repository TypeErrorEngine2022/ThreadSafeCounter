#include "headers/threadsafe-counter-sub.h"
#include "headers/threadsafe-counter.h"
#include <algorithm>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

// Function to run the benchmark
template <typename CounterType>
std::chrono::microseconds benchmark_counter(CounterType& counter,
											int num_threads,
											int operations_per_thread,
											std::function<void(CounterType&, int)> worker_func)
{
	std::vector<std::thread> threads;
	auto start = std::chrono::high_resolution_clock::now();

	// Start the threads
	for(int i = 0; i < num_threads; ++i)
	{
		threads.emplace_back(worker_func, std::ref(counter), operations_per_thread);
	}

	// Wait for threads to complete
	for(auto& thread : threads)
	{
		thread.join();
	}

	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

// Worker functions for each counter type
void ThreadSafeCounter_worker(ThreadSafeCounter& counter, int operations)
{
	for(int i = 0; i < operations; ++i)
	{
		counter.increment();
	}
}

void ThreadSafeCounterWithSubCounter_worker(ThreadSafeCounterWithSubCounter& counter, int operations)
{
	auto it = counter.register_counter();
	for(int i = 0; i < operations; ++i)
	{
		counter.increment(it);
	}
}

// Run multiple iterations and collect statistics
template <typename CounterType>
std::vector<std::chrono::microseconds>
run_benchmark_iterations(int iterations,
						 int num_threads,
						 int operations_per_thread,
						 std::function<void(CounterType&, int)> worker_func)
{
	std::vector<std::chrono::microseconds> results;
	results.reserve(iterations);

	for(int i = 0; i < iterations; ++i)
	{
		CounterType counter;
		auto duration = benchmark_counter(counter, num_threads, operations_per_thread, worker_func);
		results.push_back(duration);
	}

	return results;
}

// Calculate statistics
struct BenchmarkStats
{
	std::chrono::microseconds min_time;
	std::chrono::microseconds max_time;
	std::chrono::microseconds avg_time;
	std::chrono::microseconds median_time;
};

BenchmarkStats calculate_stats(std::vector<std::chrono::microseconds> durations)
{
	// Sort for min, max, median
	std::sort(durations.begin(), durations.end());

	// Calculate average
	auto sum = std::accumulate(durations.begin(), durations.end(), std::chrono::microseconds(0));
	auto avg = std::chrono::microseconds(sum.count() / durations.size());

	// Calculate median
	std::chrono::microseconds median;
	if(durations.size() % 2 == 0)
	{
		median = std::chrono::microseconds((durations[durations.size() / 2 - 1].count() +
											durations[durations.size() / 2].count()) /
										   2);
	}
	else
	{
		median = durations[durations.size() / 2];
	}

	return {durations.front(), durations.back(), avg, median};
}

void print_stats(const std::string& title, const BenchmarkStats& stats)
{
	std::cout << title << ":\n";
	std::cout << "  Min time: " << stats.min_time.count() << " µs\n";
	std::cout << "  Max time: " << stats.max_time.count() << " µs\n";
	std::cout << "  Avg time: " << stats.avg_time.count() << " µs\n";
	std::cout << "  Median:   " << stats.median_time.count() << " µs\n";
	std::cout << std::endl;
}

void compare_performance(const BenchmarkStats& ThreadSafeCounter_stats, const BenchmarkStats& ThreadSafeCounterWithSubCounter_stats)
{
	double speedup = static_cast<double>(ThreadSafeCounter_stats.median_time.count()) /
					 static_cast<double>(ThreadSafeCounterWithSubCounter_stats.median_time.count());

	std::cout << "Performance comparison:\n";
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "  Speedup ratio (ThreadSafeCounter/ThreadSafeCounterWithSubCounter): " << speedup << "x\n";

	if(speedup > 1.0)
	{
		std::cout << "  The ThreadSafeCounterWithSubCounter implementation is " << (speedup - 1.0) * 100.0
				  << "% faster\n";
	}
	else if(speedup < 1.0)
	{
		std::cout << "  The ThreadSafeCounter implementation is " << (1.0 / speedup - 1.0) * 100.0
				  << "% faster\n";
	}
	else
	{
		std::cout << "  Both implementations have similar performance\n";
	}
	std::cout << std::endl;
}

int main()
{
	int thread_counts[] = {1, 2, 4, 8, 16};
	int operations_per_thread = 100000; // Adjust as needed
	int iterations = 5; // Run each benchmark 5 times

	std::cout << "============================================\n";
	std::cout << "Counter Performance Benchmark\n";
	std::cout << "Operations per thread: " << operations_per_thread << "\n";
	std::cout << "Iterations per test: " << iterations << "\n";
	std::cout << "============================================\n\n";

	for(int num_threads : thread_counts)
	{
		std::cout << "Testing with " << num_threads << " threads:\n";
		std::cout << "--------------------------------------------\n";

		// Run ThreadSafeCounter counter benchmark
		auto ThreadSafeCounter_results = run_benchmark_iterations<ThreadSafeCounter>(
			iterations, num_threads, operations_per_thread, ThreadSafeCounter_worker);

		// Run ThreadSafeCounterWithSubCounter benchmark
		auto ThreadSafeCounterWithSubCounter_results = run_benchmark_iterations<ThreadSafeCounterWithSubCounter>(
			iterations, num_threads, operations_per_thread, ThreadSafeCounterWithSubCounter_worker);

		// Calculate and print statistics
		auto ThreadSafeCounter_stats = calculate_stats(ThreadSafeCounter_results);
		auto ThreadSafeCounterWithSubCounter_stats = calculate_stats(ThreadSafeCounterWithSubCounter_results);

		print_stats("ThreadSafeCounter", ThreadSafeCounter_stats);
		print_stats("ThreadSafeCounterWithSubCounter", ThreadSafeCounterWithSubCounter_stats);

		compare_performance(ThreadSafeCounter_stats, ThreadSafeCounterWithSubCounter_stats);
		std::cout << "============================================\n\n";
	}

	return 0;
}