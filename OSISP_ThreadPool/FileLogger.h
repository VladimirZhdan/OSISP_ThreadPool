#pragma once

#include <Windows.h>
#include <string>
#include <ctime>
#include <fstream>


using namespace std;

class FileLogger
{
public:
	static void Log(string message);
private:
	static HANDLE hMutex;
	static string logFileName;
};

