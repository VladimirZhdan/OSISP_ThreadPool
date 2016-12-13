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

	void StartThreadManager();
	ThreadTask *DequeueTask();
	static DWORD WINAPI ThreadManagerMethod(PVOID params);
	static DWORD WINAPI WorkThreadMethod(PVOID params);
	void AddThread();	
	ThreadInfo* GetThreadInfo(DWORD threadId);
	ThreadInfo *GetFreeThread();
	int GetFreeThreadCount();
	void ExpandThreadCount();
	void StopUnusedThread();
	void RemoveThreadFromThreadVector(DWORD threadId);

	// Fields
	
	int expandRequiredCount;
	const int minThreadCount;
	const int maxThreadCount;
	bool isStoppedThreads;
	bool requireStopManager;
	HANDLE stopManagerEvent;
	HANDLE requiredManagerEvent;
	CRITICAL_SECTION taskVectorCS;
	CRITICAL_SECTION threadVectorCS;
	std::vector<ThreadTask*> taskVector;
	std::vector<ThreadInfo*> threadVector;
};

