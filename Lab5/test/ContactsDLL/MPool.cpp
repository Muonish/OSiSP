#include "stdafx.h"
#include "MPool.h"


MPool::MPool(void)
{
	semaphore = CreateSemaphore( NULL , 0, 1000, NULL);
	mutex = CreateMutex(NULL, false ,NULL);

	for( int i = 0; i < 10; i++ )
	{
		threads[i] = CreateThread(NULL, 0, MPool::ThreadFunction, this, 0, NULL);		
	}
}

MPool::~MPool(void)
{
	CloseHandle(semaphore);
	CloseHandle(mutex);
	for( int i = 0; i < 10; i++ )
	{
		TerminateThread(threads[i], NULL);
		CloseHandle(threads[i]);
	}
}

long MPool::AddTask(FuncType newTask, void* newParameter)
{
	long count;
	long result = WaitForSingleObject(mutex, INFINITE);
	if( result != WAIT_OBJECT_0 )
		return -1;
	tasks.push(newTask);
	parameters.push(newParameter);
	ReleaseMutex(mutex);
	ReleaseSemaphore(semaphore, 1, &count);
	return count;
}

DWORD WINAPI  MPool::ThreadFunction(PVOID pvParam)
{
	MPool *me = (MPool*)pvParam;
	while(true)
	{
		WaitForSingleObject(me->semaphore, INFINITE);
		WaitForSingleObject(me->mutex, INFINITE);
		FuncType F = me->tasks.front();
		me->tasks.pop();
		void *parameter = me->parameters.front();
			me->parameters.pop();
		ReleaseMutex(me->mutex);
		F(parameter);
	}
	return 0;
}
