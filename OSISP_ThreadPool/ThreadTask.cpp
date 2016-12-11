#include "stdafx.h"
#include "ThreadTask.h"

ThreadTask::ThreadTask(int(*task)(PVOID params), PVOID params)
{
	this->task = task;
	this->params = params;
}

int ThreadTask::Run()
{
	if (this->task != NULL)
	{
		return task(params);
	}
	else
	{
		return -1;
	}
}


ThreadTask::~ThreadTask()
{
}
