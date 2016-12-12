#pragma once

#include <Windows.h>
#include "FileLogger.h"

class ThreadTask
{
public:
	ThreadTask(int(*task)(PVOID params), PVOID params);
	int Run();
	~ThreadTask();
private:
	PVOID params;
	int(*task)(PVOID params);

};

