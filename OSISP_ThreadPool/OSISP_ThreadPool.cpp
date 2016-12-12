// OSISP_ThreadPool.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <locale>
#include <Windows.h>
#include <string>
#include <iostream>
#include "ThreadPool.h"
#include "ThreadTask.h"
#include "FileLogger.h"

static int ThreadFunc(PVOID params)
{
	std::string *message = reinterpret_cast<std::string *>(params);
	Sleep(500);
	std::cout << (*message);
	Sleep(500);
	delete message;
	return 0;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	FileLogger::ClearFile();
	ThreadPool *threadPool = new ThreadPool(5);	

	for (int i = 0; i < 5; ++i)
	{
		std::string mess = "Поток №";	
		mess.append(std::to_string(i));
		mess.append("\n");

		std::string *message = new std::string(mess);
		threadPool->EnqueueTask(new ThreadTask(ThreadFunc, (PVOID)(message)));
	}

	system("pause");
	threadPool->StopAllThread();

	delete threadPool;
	
    return 0;
}

