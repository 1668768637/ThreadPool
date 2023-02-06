#include"Task.h"
#include<Windows.h>

std::atomic_int Task::number = 0;
std::mutex Task::cout_mtx;

Task::Task()
{
	number++;
	num = number;
}
Task::Task(const Task& t)
{
	num = t.num;
}
Task::~Task()
{

}

void Task::run()
{
	Task::cout_mtx.lock();
	std::cout << num << std::endl;
	Task::cout_mtx.unlock();
}