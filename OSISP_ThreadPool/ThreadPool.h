#pragma once

#include <vector>
#include "ThreadTask.h"
#include "FileLogger.h"
#include "ThreadInfo.h"

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
	static DWORD WINAPI ThreadManagerMethod(PVOID params);
	static DWORD WINAPI WorkThreadMethod(PVOID params);
	void AddThread();

	// Fields
	
	const int minThreadCount;
	bool isStoppedThreads;
	CRITICAL_SECTION taskVectorCS;
	CRITICAL_SECTION threadVectorCS;
	std::vector<ThreadTask*> taskVector;
	std::vector<ThreadInfo*> threadVector;
};

