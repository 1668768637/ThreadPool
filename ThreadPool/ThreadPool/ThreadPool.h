#pragma once
#include <iostream>
#include <Windows.h>
#include <Thread>
#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include"Task.h"

class ThreadPool {
public:
	ThreadPool(int nums);
	~ThreadPool();

	void pushTask(Task& t);

	void start();

	void stop();
private:
	std::atomic_bool isRunning;
	std::atomic_bool isEnd;
	std::vector<std::thread> threadPool;
	std::queue<Task> tasks;
	std::mutex tpPoll_mtx, isRunning_mtx, isEnd_mtx, tasks_mtx;
	std::condition_variable cv;

	void work();
};