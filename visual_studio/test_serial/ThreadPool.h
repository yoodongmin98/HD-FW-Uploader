#pragma once
#include "pch.h"




class ThreadPool
{
public:
	static ThreadPool* TP;
	ThreadPool();
	ThreadPool(size_t numThreads);

	~ThreadPool();


	void AddWork(std::function<void()> _function);
	std::queue<std::function<void()>> GetTasks()
	{
		return tasks;
	}
	std::vector<std::thread>& GetWorkers()
	{
		return Worker;
	}
	void Resize(size_t numThreads);
protected:
	void WorkerThread();
private:
	//돌아갈 쓰레드들
	std::vector<std::thread> Worker;
	std::mutex QueueMutex;
	std::queue<std::function<void()>> tasks;
	std::condition_variable condition;
	std::atomic<bool> stop;
};