#include "stdafx.h"
#include "CriticalSectionManager.h"


CriticalSectionManager::CriticalSectionManager()
{
	InitializeCriticalSection(&this->criticalSection);
}


CriticalSectionManager::~CriticalSectionManager()
{
	DeleteCriticalSection(&this->criticalSection);
}

void CriticalSectionManager::Lock()
{
	EnterCriticalSection(&this->criticalSection);
}

void CriticalSectionManager::UnLock()
{
	LeaveCriticalSection(&this->criticalSection);
}
