#pragma once

#include <vector>
#include "ThreadTask.h"

class ThreadPool
{
public:
	ThreadPool(int size);
	void EnqueueTask(ThreadTask *task);
	void StopAllThread();
	~ThreadPool();
private:
	// Methods
	
	ThreadTask *DequeueTask();
	static DWORD WINAPI WorkThreadMethod(PVOID params);
	void AddThread();

	// Fields
	
	bool isStoppedThreads;
	CRITICAL_SECTION taskVectorCS;
	CRITICAL_SECTION threadVectorCS;
	std::vector<ThreadTask*> taskVector;
	std::vector<HANDLE> threadVector;
};

