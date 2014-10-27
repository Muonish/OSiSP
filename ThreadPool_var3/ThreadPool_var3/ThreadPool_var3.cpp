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

#define _ELPP_THREAD_SAFE
#include "easylogging++.h"

#define DEFAULT_N_THREAD 10

void TaskFunction(void*);
static DWORD WINAPI MainThreadFunction(PVOID pvParam);
ThreadPool *pool;
int NthreadMax = 0;
int NthreadMin = 0;
int Ntask;

_INITIALIZE_EASYLOGGINGPP

int _tmain(int argc, _TCHAR* argv[])
{

	srand(time(NULL)); 
	LOG(INFO) << "================================================";
	std::cout << "Enter the minimum number of threads: ";
	std::cin >> NthreadMin;
	if (NthreadMin == 0) 
		NthreadMin = DEFAULT_N_THREAD;
	while (NthreadMax < NthreadMin)
	{
		std::cout << "Enter the maximum number of threads: ";
		std::cin >> NthreadMax;
		if (NthreadMax == 0)
			NthreadMax = NthreadMin * 2; 
	}
	std::cout << "Enter the number of tasks: ";
	std::cin >> Ntask;

	pool = new ThreadPool(NthreadMin, NthreadMax); 
	int *mas = new int[Ntask];

	for( int i = 0; i < Ntask; i++)
		mas[i] = i+1;
	for( int i = 0; i < Ntask; i++)
	{
		pool->taskd->add(TaskFunction, &mas[i]);
		LOG(INFO) << "id " << std::to_string(GetCurrentThreadId()) << ": task number " << std::to_string(mas[i]) << " added to the queue";
	}
	_getch();

	delete pool;
	delete[] mas;
	//HANDLE thread = CreateThread(NULL, 0, MainThreadFunction, NULL, 0, NULL);

	_getch();
	return 0;
}

void TaskFunction(void* param)
{
	int *i = (int*)param;
	int runtime = rand() % 10;

	if (runtime == 0)
	{
		LOG(INFO) << "id " << std::to_string(GetCurrentThreadId()) << ": ERROR: task number " << std::to_string(*i) << " is failed";
	}
	else
	{
		Sleep(runtime * 10);
		LOG(INFO) << "id " << std::to_string(GetCurrentThreadId()) << ": task number " << std::to_string(*i) << " is complited";
	}
}

//DWORD WINAPI MainThreadFunction(PVOID pvParam)
//{
//	pool = new ThreadPool(NthreadMin, NthreadMax); 
//	int *mas = new int[Ntask];
//
//	for( int i = 0; i < Ntask; i++)
//		mas[i] = i+1;
//	for( int i = 0; i < Ntask; i++)
//	{
//		pool->AddTask(TaskFunction, &mas[i]);
//		LOG(INFO) << "id " << std::to_string(GetCurrentThreadId()) << ": task number " << std::to_string(mas[i]) << " added to the queue";
//	}
//	_getch();
//
//	delete pool;
//	delete[] mas;
//
//	return 0 ;
//}
