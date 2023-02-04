#include <iostream>
#include <Windows.h>
#include <Thread>
#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

std::mutex cout_mtx,isRunning_mtx,isEnd_mtx,tasks_mtx;


class Task {
public:
	Task() {
		number++;
		num = number;
	}
	virtual ~Task()
	{

	}
	virtual void run()
	{
		cout_mtx.lock();

		std::cout << "Hello world!" << num << std::endl;

		cout_mtx.unlock();
	}

	
private:
	static int number;
	int num;
};
int Task::number = 0;

class ThreadPool {
public:
	ThreadPool(int nums) : isRunning(true), isEnd(false){
		for (int i = 0; i < nums; i++)
		{
			threadPool.emplace_back(&ThreadPool::work, this);
		}
	}
	~ThreadPool() {
		while (!threadPool.empty())
		{
			threadPool.pop_back();
		}
	}

	void pushTask(Task& t) {
		std::unique_lock<std::mutex> ulock(this->tpPoll_mtx);
		this->tasks.push(t);
		ulock.unlock();

		cv.notify_one();

	}

	void start() {
		cv.notify_all();
		isRunning_mtx.lock();
		isRunning = true;
		isRunning_mtx.unlock();
	}

	void stop() {
		isEnd_mtx.lock();
		isEnd = true;
		isEnd_mtx.unlock();
		tasks_mtx.lock();
		bool taskEmpty = tasks.empty();
		tasks_mtx.unlock();
		while (!this->tasks.empty())
		{
			cout_mtx.lock();
			std::cout << "还有线程在运行!" << std::endl;
			cout_mtx.unlock();
			Sleep(500);
		}

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
		std::cout << "stop";
	}
private:
	std::atomic_bool isRunning;
	std::atomic_bool isEnd;
	std::vector<std::thread> threadPool;
	std::queue<Task> tasks;
	std::mutex tpPoll_mtx;
	std::condition_variable cv;

	void work() {
		while (1)
		{
			Task* task = nullptr;
			{
				std::unique_lock<std::mutex> ulock(this->tpPoll_mtx);
				while(this->tasks.empty())
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
					task = &this->tasks.front();
					this->tasks.pop();
				}
				ulock.unlock();
			}

			if (task != nullptr)task->run();
			Sleep(200);

		}
		cout_mtx.lock();
		std::cout << "线程结束" << std::endl;
		cout_mtx.unlock();
	}
};


int main()
{
	ThreadPool pool(5);
	Task t[25];

	for (Task& task : t)
	{
		pool.pushTask(task);
	}
	pool.stop();
	std::cout << "aaa";
	getchar();
	return 0;
}