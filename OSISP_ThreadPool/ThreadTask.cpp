#include "stdafx.h"
#include "ThreadTask.h"

ThreadTask::ThreadTask(int(*task)(PVOID params), PVOID params)
{
	this->task = task;
	this->params = params;
}

int ThreadTask::Run()
{
	int result = -1;
	if (this->task != NULL)
	{
		try
		{
			result = task(params);
		}
		catch (...)
		{
			FileLogger::Log("Error during performing task");
		}
	}
	return result;
}


ThreadTask::~ThreadTask()
{
}
