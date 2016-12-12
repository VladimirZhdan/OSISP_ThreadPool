#pragma once

#include <Windows.h>

class ThreadInfo
{
public:
	ThreadInfo(HANDLE threadHandle, DWORD threadId);	
	~ThreadInfo();
	HANDLE threadHandle;
	DWORD threadId;
	HANDLE executeTaskEvent;	
};

