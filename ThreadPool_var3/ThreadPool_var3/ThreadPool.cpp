#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(int min, int max)
{
	NthreadMin = min;
	NthreadMax = max;
	
	LOG(INFO) << "Minimum number of threads = " << to_string(NthreadMin);
	LOG(INFO) << "Maximum number of threads = " << to_string(NthreadMax);

	queueMutex = CreateMutex(NULL, false, NULL);
	notEmptySemaphore = CreateSemaphore( NULL , 0, 1000, NULL);

	taskd = new TaskDispatcher(this);
	workd = new WorkerDispatcher(this);

}

ThreadPool::~ThreadPool(void)
{
	//for( int i = 0; i < NthreadMin; i++ )
	//{
	//	TerminateThread(threads[i], NULL);
	//	CloseHandle(threads[i]);
	//}
	delete taskd;
	delete workd;
	CloseHandle(queueMutex);
	CloseHandle(notEmptySemaphore);
}

ThreadPool::TaskDispatcher::TaskDispatcher(ThreadPool *p)
{
	parent = p;
}

ThreadPool::TaskDispatcher::~TaskDispatcher(void)
{
}

bool ThreadPool::TaskDispatcher::add(FuncType newTask, void* newParameter)
{
	long oldState = 0;

	WaitForSingleObject(parent->queueMutex, INFINITE);

	TASKINFO newInfo;
	newInfo.userFunction = newTask;
	newInfo.param = newParameter;

	parent->tasks.push(newInfo);
	
	ReleaseSemaphore(parent->notEmptySemaphore, 1, &oldState);
	ReleaseMutex(parent->queueMutex);
	return true;
}

/* WORKER DISPATCHER */

ThreadPool::WorkerDispatcher::WorkerDispatcher(ThreadPool *p)
{
	parent = p;

	TASKINFO emptyTask = {0};
	for (int i = 0; i < p->NthreadMin; i++)
		createWorker(&emptyTask);

	threadMain = CreateThread(NULL, 0, ThreadPool::WorkerDispatcher::threadWorkerDispatcher, this, 0, NULL);
	LOG(INFO) << "=============Thread pool is created=============";
}

ThreadPool::WorkerDispatcher::~WorkerDispatcher(void)
{
	for (int i = 0; i < threads.size(); i++)
	{
		killWorker(&threads[i]);
		threads.erase(threads.begin() + i);
		i--;
	}
	TerminateThread(threadMain, NULL);
	CloseHandle(threadMain);
}

TASKINFO ThreadPool::WorkerDispatcher::requestTask()
{
	TASKINFO result = {0};
	
	WaitForSingleObject(parent->queueMutex, INFINITE);

	if (!parent->tasks.empty())
	{
		result = parent->tasks.front();
		parent->tasks.pop();
	}
	else
		LOG(WARNING) << "QUEUE IS EMPTY!";
	
	ReleaseMutex(parent->queueMutex);
	return result;
}

void ThreadPool::WorkerDispatcher::lookAroundWorkers()
{
	int result = 0;

	LOG(INFO) << "!!!!!!!!!!!!!lookAroundWorkers!!!!!!!!!!!!!!!";
	for (int i = 0; i < threads.size(); i++)
	{
		if ((threads[i].currentState == DEAD) || ( threads.size() > parent->NthreadMin&& threads[i].currentState == CAN_WORK) )
		switch (threads[i].currentState)
		{
		case DEAD:
			killWorker(&threads[i]);
			threads.erase(threads.begin() + i);
			i--;
			break;
		case CAN_WORK:
			if (threads.size() > parent->NthreadMin)
				threads[i].currentState = WILL_DIE;
		}
	}
}

void ThreadPool::WorkerDispatcher::dispatch(TASKINFO *newTask)
{
	bool noFreeWorkers = true;
	long oldState = 0;

	LOG(INFO) << "!!!!!!!!!!!!!dispatch!!!!!!!!!!!!!!!";
	for (int i = 0; i < threads.size(); i++)
	{
		if (threads[i].currentState == CAN_WORK || threads[i].currentState == WILL_DIE )
		{
			threads[i].task = *newTask;
			ReleaseSemaphore(threads[i].semaphore, 1, &oldState);
			noFreeWorkers = false;
		}
	}
	if (noFreeWorkers)
		createWorker(newTask);
}

void ThreadPool::WorkerDispatcher::createWorker(TASKINFO *newTask)
{
	WORKER tmp = { 0 };

	threads.push_back(tmp);

	threads.back().task.userFunction = newTask->userFunction;
	threads.back().task.param = newTask->param;
	threads.back().currentState = CAN_WORK;
	threads.back().semaphore = CreateSemaphore( NULL , 0, 1000, NULL);
	threads.back().thread = CreateThread(NULL, 0, ThreadPool::WorkerDispatcher::threadWorker, &(threads.back()), 0, NULL);
	LOG(INFO) << "id "<< GetCurrentThreadId() << ": create thread: id = " << GetThreadId(threads.back().thread);
	
	if (threads.size() > parent->NthreadMax)
			LOG(WARNING) << "EXCEEDED THE MAX N OF THREADS!";
}

void ThreadPool::WorkerDispatcher::killWorker(WORKER *target)
{
	CloseHandle(target->semaphore);
	CloseHandle(target->thread);
}

DWORD WINAPI ThreadPool::WorkerDispatcher::threadWorkerDispatcher(PVOID pvParam)
{
	ThreadPool::WorkerDispatcher *me = (ThreadPool::WorkerDispatcher *)pvParam;
	ThreadPool *parent = me->parent;
	TASKINFO currentTask;

	LOG(INFO) << "id "<< GetCurrentThreadId() << ": worker dispatcher thread created";

	while (true)
	{
		WaitForSingleObject(parent->notEmptySemaphore, INFINITE);

		/*for (int i = 0; i < me->threads.size(); i++)
		{
			LOG(INFO) << i << ": curState=" << me->threads[i].currentState << " : thread=" << me->threads[i].thread; 
		}*/
		currentTask = me->requestTask();
		me->lookAroundWorkers();
		me->dispatch(&currentTask);
	}
	return 0;
}

DWORD WINAPI ThreadPool::WorkerDispatcher::threadWorker(PVOID pvParam)
{
	ThreadPool::WorkerDispatcher::WORKER *me = (ThreadPool::WorkerDispatcher::WORKER *)pvParam;

	while (true)
	{
		LOG(INFO) << "!!!!!!!!!!!!!threadWorker!!!!!!!!!!!!!!!";
		State instruction = me->currentState;  //save instruction from WD
		me->currentState = BUSY;

		FuncType F = me->task.userFunction;
		if (F != NULL)	
			F(me->task.param);

		if (instruction == WILL_DIE)
		{
			me->currentState = DEAD;
			return 0;
		}

		me->currentState = CAN_WORK;
		WaitForSingleObject(me->semaphore, INFINITE);   //wait for next task
	}

	return 0;
}

