#pragma once

#include <Windows.h>
#include <string>
#include <ctime>
#include <fstream>
#include "CriticalSectionManager.h"


using namespace std;

class FileLogger
{
public:
	static void Log(string message);
	static void ClearFile();
private:	
	static CriticalSectionManager managerCS;	
	static string logFileName;	
};
