#pragma once
#include<iostream>
#include <mutex>
#include <atomic>

class Task {
public:
	Task();
	Task(const Task&);
	virtual ~Task();
	virtual void run();


private:
	static std::atomic_int number;
	static std::mutex cout_mtx;
	int num;
};