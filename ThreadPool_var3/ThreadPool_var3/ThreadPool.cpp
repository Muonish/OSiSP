#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(int n)
{
	logFile.open("log.txt");
	Nthread = n;
	mutex = CreateMutex(NULL, false ,NULL);
	mutexLog = CreateMutex(NULL, false ,NULL);
	threadCounter = 0;
	AddLog("Number of threads = " + to_string(Nthread));
	for( int i = 0; i < Nthread; i++ )
	{
		threads.push_back(CreateThread(NULL, 0, ThreadPool::ThreadFunction, this, 0, NULL));		
		AddLog("id " + to_string(GetCurrentThreadId()) + ": create thread id = " + to_string(GetThreadId(threads[i])));
	}
	AddLog("=============Thread pool is created=============");
}

ThreadPool::~ThreadPool(void)
{
	CloseHandle(mutex);
	CloseHandle(mutexLog);
	logFile.close();
	for( int i = 0; i < Nthread; i++ )
	{
		TerminateThread(threads[i], NULL);
		CloseHandle(threads[i]);
	}
}

long ThreadPool::AddTask(FuncType newTask, void* newParameter)
{
	long result = WaitForSingleObject(mutex, 10000000);
	if( result != WAIT_OBJECT_0 )
		return -1;

	tasks.push(newTask);
	parameters.push(newParameter);

	if (threadCounter == Nthread)
	{
		AddLog("id " + to_string(GetCurrentThreadId()) + ": WARNING: all threads are busy");
		AddThread();

	}
	ReleaseMutex(mutex);
	return 0;
}
void ThreadPool::AddThread(void)
{
	threads.push_back(CreateThread(NULL, 0, ThreadPool::ThreadFunction, this, 0, NULL));
	AddLog("id " + to_string(GetCurrentThreadId()) + ": create thread id = " + to_string(GetThreadId(threads.back())));
	Nthread++;
}

void ThreadPool::AddLog(string s)
{
	WaitForSingleObject(mutexLog, INFINITE);
	logFile << s << endl;
	ReleaseMutex(mutexLog);
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
		}
		FuncType F = me->tasks.front();
		me->tasks.pop();
		void *parameter = me->parameters.front();
		me->parameters.pop();

		me->threadCounter++;
		ReleaseMutex(me->mutex);
		F(parameter);

		WaitForSingleObject(me->mutex, INFINITE);
		me->threadCounter--;
		ReleaseMutex(me->mutex);
	}
	return 0;
}
