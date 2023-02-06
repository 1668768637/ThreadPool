#include"ThreadPool.h"

ThreadPool::ThreadPool(int nums) : isRunning(true), isEnd(false)
{
	for (int i = 0; i < nums; i++)
	{
		threadPool.emplace_back(&ThreadPool::work, this);
	}
}
ThreadPool::~ThreadPool() 
{
	while (!threadPool.empty())
	{
		threadPool.pop_back();
	}
}

void ThreadPool::pushTask(Task& t) 
{
	tasks_mtx.lock();
	this->tasks.push(t);
	tasks_mtx.unlock();

	cv.notify_one();

}

void ThreadPool::start() 
{
	cv.notify_all();
	isRunning_mtx.lock();
	isRunning = true;
	isRunning_mtx.unlock();
}

void ThreadPool::stop() 
{
	isEnd_mtx.lock();
	isEnd = true;
	isEnd_mtx.unlock();
	tasks_mtx.lock();
	bool taskEmpty = tasks.empty();
	tasks_mtx.unlock();
	while (!this->tasks.empty())
		Sleep(500);

	isRunning_mtx.lock();
	isRunning = false;
	isRunning_mtx.unlock();
		

	cv.notify_all();
	for (auto& th : threadPool)
	{
		if (th.joinable())
		{
			th.join();
		}
	}
}

void ThreadPool::work() 
{
	while (1)
	{
		Task* task = nullptr;
		{
			std::unique_lock<std::mutex> ulock(this->tpPoll_mtx);
			while (this->tasks.empty())
			{
				isEnd_mtx.lock();
				bool flag_isEnd = isEnd;
				isEnd_mtx.unlock();
				if (flag_isEnd)
				{
					break;
				}
				cv.wait(ulock);
			}
			isEnd_mtx.lock();
			bool flag = isEnd;
			isEnd_mtx.unlock();
			tasks_mtx.lock();
			bool taskEmpty = tasks.empty();
			tasks_mtx.unlock();
			if (flag && taskEmpty)break;

			isRunning_mtx.lock();
			bool flag_isRun = isRunning;
			isRunning_mtx.unlock();
			if (flag_isRun && !taskEmpty)
			{
				tasks_mtx.lock();
				task = &this->tasks.front();
				this->tasks.pop();
				tasks_mtx.unlock();
			}
			ulock.unlock();
		}

		if (task != nullptr)task->run();
		//Sleep(200);

	}
}
