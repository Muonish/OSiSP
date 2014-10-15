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

//using namespace std;
void TaskFunction(void*);
static DWORD WINAPI MainThreadFunction(PVOID pvParam);
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
	long id = GetCurrentThreadId();
	int runtime = rand() % 10;

	if (runtime == 0)
	{
		printf("id %5d: ERROR: task number %3d is failed\n", id, *i);
	}
	else
	{
		Sleep(runtime * 10);
		printf("id %5d: task number %3d is complited\n", id, *i);
	}
}

DWORD WINAPI MainThreadFunction(PVOID pvParam)
{
	ThreadPool pool(Nthread); 
	long id = GetCurrentThreadId();
	int *mas = new int[Ntask];

	for( int i = 0; i < Ntask; i++)
		mas[i] = i+1;
	for( int i = 0; i < Ntask; i++)
	{
		pool.AddTask(TaskFunction, &mas[i]);
		printf("id %5d: task number %3d added to the queue\n", id, mas[i]);
		//printf("id %5d: ERROR: all threads are busy\n", id);
	}
	_getch();
	delete[] mas;

	return 0 ;
}
