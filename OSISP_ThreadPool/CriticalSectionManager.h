#pragma once

#include <Windows.h>

class CriticalSectionManager
{
public:	
	CriticalSectionManager();
	~CriticalSectionManager();
	void Lock();
	void UnLock();
private:
	CRITICAL_SECTION criticalSection;
};

