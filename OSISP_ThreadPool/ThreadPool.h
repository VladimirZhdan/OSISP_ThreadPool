#pragma once

#include <vector>
#include "ThreadTask.h"

class ThreadPool
{
public:
	ThreadPool(int size);
	~ThreadPool();
private:
	std::vector<ThreadTask*> taskVector;
};

