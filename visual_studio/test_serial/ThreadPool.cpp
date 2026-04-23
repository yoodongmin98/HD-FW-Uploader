#include "pch.h"
#include "ThreadPool.h"
#include <utility>



ThreadPool* ThreadPool::TP = nullptr;


ThreadPool::ThreadPool()
{
	TP = this;
}


ThreadPool::ThreadPool(size_t numThreads)
{
	for (size_t i = 0; i < numThreads; ++i)
	{
		Worker.push_back(std::thread(&ThreadPool::WorkerThread, this));
	}
}


ThreadPool::~ThreadPool()
{
	{
		std::lock_guard<std::mutex> lock(QueueMutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& worker : Worker)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}
}



void ThreadPool::WorkerThread()
{
	while (true)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(QueueMutex);
			condition.wait(lock, [this]() { return !tasks.empty() || stop; });

			if (stop && tasks.empty())
			{
				return;
			}


			task = std::move(tasks.front());
			tasks.pop();
			lock.unlock();
		}
		try
		{
			task();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Serial Thread 처리에 오류가 발생했습니다 : " << e.what() << std::endl;
		}
	}
}



void ThreadPool::AddWork(std::function<void()> _function)
{
	{
		std::lock_guard<std::mutex> lock(QueueMutex);
		if (stop)
		{
			throw std::runtime_error("ThreadPool is stopped.");
		}
		tasks.push(_function);
	}
	condition.notify_one();
}

void ThreadPool::Resize(size_t numThreads) {
	std::lock_guard<std::mutex> lock(QueueMutex);

	// 쓰레드 추가
	while (Worker.size() < numThreads)
	{
		Worker.push_back(std::thread(&ThreadPool::WorkerThread, this));
	}

	// 쓰레드 제거
	while (Worker.size() > numThreads)
	{
		AddWork([this]() { stop = true; });
		Worker.back().join();
		Worker.pop_back();
	}
}

