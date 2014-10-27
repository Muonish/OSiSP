#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(int min, int max)
{
	NthreadMin = min;
	NthreadMax = max;
	
	LOG(INFO) << "Minimum number of threads = " << to_string(NthreadMin);
	LOG(INFO) << "Maximum number of threads = " << to_string(NthreadMax);

	requestMutex = CreateMutex(NULL, true, NULL);

	task = new TaskDispatcher(this);

}

ThreadPool::~ThreadPool(void)
{
	//for( int i = 0; i < NthreadMin; i++ )
	//{
	//	TerminateThread(threads[i], NULL);
	//	CloseHandle(threads[i]);
	//}
}

//void ThreadPool::AddThread(void)
//{
//	threads.push_back(CreateThread(NULL, 0, ThreadPool::ThreadFunction, this, 0, NULL));
//	LOG(INFO) << "id " << to_string(GetCurrentThreadId()) << ": create thread id = " << to_string(GetThreadId(threads.back()));
//	threadCounter++;
//}





ThreadPool::TaskDispatcher::TaskDispatcher(ThreadPool *p)
{
	parent = p;
	addMutex = CreateMutex(NULL, false ,NULL);
}

ThreadPool::TaskDispatcher::~TaskDispatcher(void)
{
	CloseHandle(addMutex);
}

bool ThreadPool::TaskDispatcher::add(FuncType newTask, void* newParameter)
{
	long result = WaitForSingleObject(addMutex, INFINITE);

	if( result != WAIT_OBJECT_0 )
	{
		LOG(ERROR) << "mutex error";
		return false;
	}

	TASKINFO newInfo;
	newInfo.task = newTask;
	newInfo.param = newParameter;

	tasks.push(newInfo);

	ReleaseMutex(parent->requestMutex);
	ReleaseMutex(addMutex);
	return true;
}

TASKINFO ThreadPool::TaskDispatcher::getTask()
{
	TASKINFO result = { 0 };
	
	WaitForSingleObject(addMutex, INFINITE);

	if (!tasks.empty())
	{
		result = tasks.front();
		tasks.pop();
	}
	else
		LOG(WARNING) << "QUEUE IS EMPTY!";
	
	ReleaseMutex(addMutex);
	return result;
}


/* WORKER DISPATCHER */

ThreadPool::WorkerDispatcher::WorkerDispatcher(ThreadPool *p)
{
	parent = p;


	threadMain = CreateThread(NULL, 0, ThreadPool::WorkerDispatcher::threadWorkerDispatcher, this, 0, NULL);
	//HARDCODE: CREATE MIN COUNT OF THREADS
}

ThreadPool::WorkerDispatcher::~WorkerDispatcher(void)
{
	//HARDCODE: DELETE ALL THREADS WORKERS
	TerminateThread(threadMain, NULL);
	CloseHandle(threadMain);
}

DWORD WINAPI ThreadPool::WorkerDispatcher::threadWorkerDispatcher(PVOID pvParam)
{
	ThreadPool::WorkerDispatcher *me = (ThreadPool::WorkerDispatcher *)pvParam;
	ThreadPool *parent = me->parent;
	TASKINFO currentTask;

	while (true)
	{
		WaitForSingleObject(parent->requestMutex, INFINITE);

		currentTask = parent->task->getTask();
		//HARDCODE: CALL me->dispatch(currentTask);
	}
}







//DWORD WINAPI ThreadPool::ThreadFunction(PVOID pvParam)
//{
//	ThreadPool *me = (ThreadPool*)pvParam;
//	long id = GetCurrentThreadId();
//	while(true)
//	{
//		WaitForSingleObject(me->mutex, INFINITE);
//
//		if (me->tasks.empty())
//		{
//			ReleaseMutex(me->mutex);
//			continue;
//		}
//		FuncType F = me->tasks.front();
//		me->tasks.pop();
//		void *parameter = me->parameters.front();
//		me->parameters.pop();
//
//		me->threadCounter++;
//		ReleaseMutex(me->mutex);
//		F(parameter);
//
//		WaitForSingleObject(me->mutex, INFINITE);
//		me->threadCounter--;
//		ReleaseMutex(me->mutex);
//	}
//	return 0;
//}


