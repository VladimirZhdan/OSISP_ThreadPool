#include "stdafx.h"
#include "FileLogger.h"

string FileLogger::logFileName = "Log.txt";
CriticalSectionManager FileLogger::managerCS;

void FileLogger::Log(string message)
{
	time_t currTime = time(NULL);
	char bufferTime[256];	
	ctime_s(bufferTime, 256, &currTime);
	string currTimeStr = bufferTime;
	currTimeStr.pop_back(); //delete "\n" from string

	managerCS.Lock();
	
	ofstream outputStream(logFileName, ofstream::app);	
	try
	{
		if (outputStream.is_open())
		{
			outputStream << currTimeStr << ": " << message << "\n";
		}
	}
	catch (...)
	{
		outputStream.close();
		throw;
	}
	outputStream.close();

	managerCS.UnLock();
}

void FileLogger::ClearFile()
{
	ofstream fileStream(logFileName, ofstream::trunc);
	fileStream.close();
}