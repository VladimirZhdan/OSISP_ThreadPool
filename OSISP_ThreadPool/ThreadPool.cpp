#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(int sizePool)
{
	InitializeCriticalSection(&taskVectorCS);
	InitializeCriticalSection(&threadVectorCS);
	isStoppedThreads = false;
	
	for (int i = 0; i < sizePool; ++i)
	{
		AddThread();
	}
}

void ThreadPool::EnqueueTask(ThreadTask * task)
{
	EnterCriticalSection(&taskVectorCS);
	taskVector.push_back(task);
	LeaveCriticalSection(&taskVectorCS);
}

void ThreadPool::StopAllThread()
{
	for (int i = 0; i < (int)threadVector.size(); ++i)
	{
		EnqueueTask(NULL);
	}
	for (int i = 0; i < (int)threadVector.size(); ++i)
	{
		WaitForSingleObject(threadVector[i], INFINITE);
	}
	isStoppedThreads = true;
}


ThreadPool::~ThreadPool()
{
	if (!isStoppedThreads)
	{
		StopAllThread();
	}
}

ThreadTask * ThreadPool::DequeueTask()
{
	ThreadTask *task = NULL;
	EnterCriticalSection(&taskVectorCS);

	while (taskVector.size() == 0)
	{
		LeaveCriticalSection(&taskVectorCS);
		Sleep(100);
		EnterCriticalSection(&taskVectorCS);
	}	
	task = taskVector[0];
	taskVector.erase(taskVector.begin());
	
	LeaveCriticalSection(&taskVectorCS);
	return task;
}

DWORD WINAPI ThreadPool::WorkThreadMethod(PVOID params)
{
	ThreadPool *thisObj = (ThreadPool*)params;
	ThreadTask *workedTask;
	while (true)
	{
		try
		{
			workedTask = thisObj->DequeueTask();			
			if (workedTask != NULL)
			{
				workedTask->Run();
				delete workedTask;
			}
			else
			{
				break;
			}
		}		
		catch (...)
		{

		}
	}

	return 0;
}

void ThreadPool::AddThread()
{
	EnterCriticalSection(&threadVectorCS);
	HANDLE thread = CreateThread(NULL, NULL, WorkThreadMethod, this, CREATE_SUSPENDED, NULL);
	threadVector.push_back(thread);	
	LeaveCriticalSection(&threadVectorCS);

	ResumeThread(thread);	
}





