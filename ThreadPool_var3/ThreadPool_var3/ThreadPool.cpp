#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(int n)
{
	int Nthread = n;
	long id = GetCurrentThreadId();
	mutex = CreateMutex(NULL, false ,NULL);

	threads = new HANDLE[Nthread];
	for( int i = 0; i < Nthread; i++ )
	{
		threads[i] = CreateThread(NULL, 0, ThreadPool::ThreadFunction, this, 0, NULL);		
	}
	printf("=============Thread pool is created=============\n");
	printf("id %5d: the number of threads is %d\n", id, Nthread);
}

ThreadPool::~ThreadPool(void)
{
	CloseHandle(mutex);
	for( int i = 0; i < 10; i++ )
	{
		TerminateThread(threads[i], NULL);
		CloseHandle(threads[i]);
	}
	delete[] threads;
}

long ThreadPool::AddTask(FuncType newTask, void* newParameter)
{
	long result = WaitForSingleObject(mutex, 10000000);
	if( result != WAIT_OBJECT_0 )
		return -1;

	tasks.push(newTask);
	parameters.push(newParameter);
	ReleaseMutex(mutex);
	return 0;
}

DWORD WINAPI  ThreadPool::ThreadFunction(PVOID pvParam)
{
	ThreadPool *me = (ThreadPool*)pvParam;
	long id = GetCurrentThreadId();
	while(true)
	{
		WaitForSingleObject(me->mutex, INFINITE);

		if (me->tasks.empty())
		{
			ReleaseMutex(me->mutex);
			continue;
			//printf("id %5d: ERROR: all threads are busy\n", id);
		}
		FuncType F = me->tasks.front();
		me->tasks.pop();
		void *parameter = me->parameters.front();
		me->parameters.pop();
		ReleaseMutex(me->mutex);
		F(parameter);
	}
	return 0;
}
