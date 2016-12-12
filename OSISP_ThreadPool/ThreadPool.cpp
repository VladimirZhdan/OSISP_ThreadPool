#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(int maxSizePool) : minThreadCount(3)
{
	isStoppedThreads = false;	
	InitializeCriticalSection(&taskVectorCS);
	InitializeCriticalSection(&threadVectorCS);	

	FileLogger::Log("Created " + std::to_string(minThreadCount) + " threads");
	for (int i = 0; i < minThreadCount; ++i)
	{		
		AddThread();		
	}	
}

void ThreadPool::EnqueueTask(ThreadTask * task)
{	
	EnterCriticalSection(&taskVectorCS);
	FileLogger::Log("Added new task");
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
		WaitForSingleObject(threadVector[i]->threadHandle, INFINITE);
	}
	isStoppedThreads = true;
}


ThreadPool::~ThreadPool()
{
	if (!isStoppedThreads)
	{
		StopAllThread();
	}
	DeleteCriticalSection(&taskVectorCS);
	DeleteCriticalSection(&threadVectorCS);
}

ThreadTask * ThreadPool::DequeueTask()
{
	ThreadTask *task = NULL;
	EnterCriticalSection(&taskVectorCS);

	while (taskVector.size() == 0)
	{
		LeaveCriticalSection(&taskVectorCS);		
		EnterCriticalSection(&taskVectorCS);
	}	
	task = taskVector[0];
	taskVector.erase(taskVector.begin());
	
	LeaveCriticalSection(&taskVectorCS);
	return task;
}

DWORD ThreadPool::ThreadManagerMethod(PVOID params)
{
	ThreadPool *thisObj = (ThreadPool*)params;
	//Ожидание сосдания минимального количества потоков

	return 0;
}

DWORD WINAPI ThreadPool::WorkThreadMethod(PVOID params)
{
	ThreadPool *thisObj = (ThreadPool*)params;
	ThreadTask *workedTask;
	while (true)
	{
		//Ожидание получения задачи
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
			FileLogger::Log("Thread catch exception: Unknown type exception");
		}
		//Сделать текущее состояние потока "Ожидающим"
		//Послать сообщение manager-у о том, что освободился поток
	}

	return 0;
}

void ThreadPool::AddThread()
{
	EnterCriticalSection(&threadVectorCS);

	DWORD threadId;
	HANDLE threadHandle = CreateThread(NULL, NULL, WorkThreadMethod, this, CREATE_SUSPENDED, &threadId);
	threadVector.push_back(new ThreadInfo(threadHandle, threadId));

	LeaveCriticalSection(&threadVectorCS);

	ResumeThread(threadHandle);
}





