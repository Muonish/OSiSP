// ThreadPool_var3.cpp: begin of console app.
//

#include "stdafx.h"
#include "conio.h"
#include "iostream"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "windows.h"
#include "process.h"
#include "ThreadPool.h"

#define DEFAULT_N_THREAD 10

void TaskFunction(void*);
static DWORD WINAPI MainThreadFunction(PVOID pvParam);
ThreadPool *pool;
int Nthread;
int Ntask;

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));
	std::cout << "Enter the number of threads: ";
	std::cin >> Nthread;
	if (Nthread == 0) 
		Nthread = DEFAULT_N_THREAD;
	std::cout << "Enter the number of tasks: ";
	std::cin >> Ntask;

	HANDLE thread = CreateThread(NULL, 0, MainThreadFunction, NULL, 0, NULL);

	_getch();
	return 0;
}

void TaskFunction(void* param)
{
	int *i = (int*)param;
	int runtime = rand() % 10;

	if (runtime == 0)
	{
		pool->AddLog("id " + std::to_string(GetCurrentThreadId()) + ": ERROR: task number " + std::to_string(*i) + " is failed");
	}
	else
	{
		Sleep(runtime * 10);
		pool->AddLog("id " + std::to_string(GetCurrentThreadId()) + ": task number " + std::to_string(*i) + " is complited");
	}
}

DWORD WINAPI MainThreadFunction(PVOID pvParam)
{
	pool = new ThreadPool(Nthread); 
	int *mas = new int[Ntask];

	for( int i = 0; i < Ntask; i++)
		mas[i] = i+1;
	for( int i = 0; i < Ntask; i++)
	{
		pool->AddTask(TaskFunction, &mas[i]);
		pool->AddLog( "id " + std::to_string(GetCurrentThreadId()) + ": task number " + std::to_string(mas[i]) + " added to the queue");
	}
	_getch();

	delete pool;
	delete[] mas;

	return 0 ;
}
