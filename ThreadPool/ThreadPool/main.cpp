#include <iostream>
#include"ThreadPool.h"

int main()
{
	ThreadPool pool(5);
	Task t[25];

	for (int i = 0; i < 25; i++)
	{
		pool.pushTask(t[i]);
	}
	pool.stop();
	std::cout << "aaa";
	getchar();
	return 0;
}