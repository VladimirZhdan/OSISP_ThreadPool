#include "stdafx.h"
#include "ThreadInfo.h"


ThreadInfo::ThreadInfo(HANDLE threadHandle, DWORD threadId)
{	
	this->threadHandle = threadHandle;
	this->threadId = threadId;
	this->executeTaskEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}


ThreadInfo::~ThreadInfo()
{
}
