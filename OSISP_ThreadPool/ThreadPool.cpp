#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(int maxSizePool) : minThreadCount(4), maxThreadCount(maxSizePool)
{
	expandRequiredCount = 0;
	isStoppedThreads = false;	
	requireStopManager = false;
	stopManagerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	requiredManagerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	InitializeCriticalSection(&taskVectorCS);
	InitializeCriticalSection(&threadVectorCS);	

	StartThreadManager();

	FileLogger::Log("Created " + std::to_string(minThreadCount) + " threads");
	for (int i = 0; i < minThreadCount; ++i)
	{		
		AddThread();		
	}	
}

void ThreadPool::EnqueueTask(ThreadTask * task)
{	
	EnterCriticalSection(&taskVectorCS);
	if (task != NULL)
	{
		FileLogger::Log("Added new task");
	}
	else
	{
		FileLogger::Log("Added new NULL task");
	}
	
	taskVector.push_back(task);
	LeaveCriticalSection(&taskVectorCS);
	
	SetEvent(requiredManagerEvent);
}

void ThreadPool::StopAllThread()
{
	requireStopManager = true;
	EnterCriticalSection(&threadVectorCS);
	for (int i = 0; i < (int)threadVector.size(); ++i)
	{
		EnqueueTask(NULL);
	}	
	LeaveCriticalSection(&threadVectorCS);	
	
	while (WaitForSingleObject(stopManagerEvent, 100) == WAIT_TIMEOUT)
	{
		SetEvent(requiredManagerEvent);
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
	CloseHandle(stopManagerEvent);
	CloseHandle(requiredManagerEvent);	
}

void ThreadPool::StartThreadManager()
{
	HANDLE threadManager = CreateThread(NULL, NULL, ThreadManagerMethod, this, 0, NULL);
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

	//Ожидание создания минимального количества потоков
	while (thisObj->GetFreeThreadCount() < thisObj->minThreadCount);

	while (true)
	{		
		if ((thisObj->taskVector.size() == 0) || (thisObj->GetFreeThreadCount() == 0))
		{
			WaitForSingleObject(thisObj->requiredManagerEvent, INFINITE);
		}

		if (thisObj->taskVector.size() > 0)
		{			
			ThreadInfo *freeThread = thisObj->GetFreeThread();
			if (freeThread != NULL)
			{
				SetEvent(freeThread->executeTaskEvent);
			}						
		}
		else
		{
			if ((thisObj->requireStopManager) && (thisObj->threadVector.size() == 0))
			{
				SetEvent(thisObj->stopManagerEvent);
				break;
			}			
			if (!thisObj->requireStopManager)
			{
				thisObj->StopUnusedThread();
			}				
		}
		ResetEvent(thisObj->requiredManagerEvent);
		if (!thisObj->requireStopManager)
		{
			if (thisObj->expandRequiredCount > 2)
			{
				thisObj->expandRequiredCount = 0;
				thisObj->ExpandThreadCount();
			}
			else
			{
				thisObj->expandRequiredCount++;
			}			
		}		
	}		

	return 0;
}

DWORD WINAPI ThreadPool::WorkThreadMethod(PVOID params)
{
	ThreadPool *thisObj = (ThreadPool*)params;
	ThreadInfo *currThreadInfo = thisObj->GetThreadInfo(GetCurrentThreadId());

	ThreadTask *workedTask;
	while (true)
	{
		//Ожидание получения задачи
		WaitForSingleObject(currThreadInfo->executeTaskEvent, INFINITE);

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
		ResetEvent(currThreadInfo->executeTaskEvent);

		//Послать сообщение manager-у о том, что освободился поток
		SetEvent(thisObj->requiredManagerEvent);
	}

	thisObj->RemoveThreadFromThreadVector(currThreadInfo->threadId);
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

ThreadInfo * ThreadPool::GetThreadInfo(DWORD threadId)
{
	ThreadInfo *result = NULL;
	for (int i = 0; i < threadVector.size(); ++i)
	{
		if (threadVector[i]->threadId == threadId)
		{
			result = threadVector[i];
		}
	}

	return result;
}

ThreadInfo * ThreadPool::GetFreeThread()
{
	ThreadInfo *result = NULL;
	EnterCriticalSection(&threadVectorCS);

	for (unsigned int i = 0; (i < threadVector.size()) && (result == NULL); ++i)
	{
		if (WaitForSingleObject(threadVector[i]->executeTaskEvent, 0) == WAIT_TIMEOUT)
		{
			result = threadVector[i];
		}
	}

	LeaveCriticalSection(&threadVectorCS);
	return result;
}

int ThreadPool::GetFreeThreadCount()
{
	int result = 0;
	EnterCriticalSection(&threadVectorCS);

	for (unsigned int i = 0; i < threadVector.size(); ++i)
	{
		if (WaitForSingleObject(threadVector[i]->executeTaskEvent, 0) == WAIT_TIMEOUT)
		{
			++result;
		}
	}

	LeaveCriticalSection(&threadVectorCS);	
	return result;
}

void ThreadPool::ExpandThreadCount()
{
	if (GetFreeThreadCount() == 0 && taskVector.size() > 0)
	{
		if (threadVector.size() < maxThreadCount)
		{
			AddThread();
		}		
	}
}

void ThreadPool::StopUnusedThread()
{
	if (GetFreeThreadCount() > 0 && taskVector.size() == 0)
	{
		if (threadVector.size() > minThreadCount)
		{
			EnqueueTask(NULL);			
		}
	}
}

void ThreadPool::RemoveThreadFromThreadVector(DWORD threadId)
{
	EnterCriticalSection(&threadVectorCS);

	bool isRemoved = false;
	for (unsigned int i = 0; (i < threadVector.size()) && (!isRemoved); ++i)
	{
		if (threadVector[i]->threadId == threadId)
		{
			delete threadVector[i];
			threadVector.erase(threadVector.begin() + i);			
			isRemoved = true;
		}
	}

	LeaveCriticalSection(&threadVectorCS);
}





