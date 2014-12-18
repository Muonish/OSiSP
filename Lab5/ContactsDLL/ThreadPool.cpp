#include "ThreadPool.h"


ThreadPool::ThreadPool(int min, int max)
{
	NthreadMin = min;
	NthreadMax = max;
	
    //LOG(INFO) << "Minimum number of threads = " << to_string(NthreadMin);
    //LOG(INFO) << "Maximum number of threads = " << to_string(NthreadMax);

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
	threads.reserve(1000);

	TASKINFO *emptyTask = new TASKINFO;
	emptyTask->param = NULL;
	emptyTask->userFunction = NULL;

	for (int i = 0; i < p->NthreadMin; i++)
		createWorker(emptyTask);

	Sleep(100);

	threadMain = CreateThread(NULL, 0, ThreadPool::WorkerDispatcher::threadWorkerDispatcher, this, 0, NULL);
    //LOG(INFO) << "=============Thread pool is created=============";
}

ThreadPool::WorkerDispatcher::~WorkerDispatcher(void)
{
	for (int i = 0; i < threads.size(); i++)
	{
		killWorker(threads[i]);
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
        ;
        //LOG(WARNING) << "QUEUE IS EMPTY!";
	
	ReleaseMutex(parent->queueMutex);
	return result;
}

void ThreadPool::WorkerDispatcher::lookAroundWorkers()
{
	int result = 0;
	int markedForDelete = 0;

	for (int i = 0; i < threads.size(); i++)
	{
		if (WaitForSingleObject(threads[i]->mutexBusy, 0) == WAIT_OBJECT_0)
		{
			bool isDead = false;
			if ((threads[i]->currentState == DEAD) || ( (parent->NthreadMin < threads.size() - markedForDelete - 1) && (threads[i]->currentState == CAN_WORK)) )
			{
				switch (threads[i]->currentState)
				{
				case DEAD:
					killWorker(threads[i]);
					threads.erase(threads.begin() + i);
					i--;
					isDead = true;
					break;
				case CAN_WORK:
					threads[i]->currentState = WILL_DIE;
					markedForDelete++;
				}
			}
			if (!isDead) 
				ReleaseMutex(threads[i]->mutexBusy);
		}
	}
}

void ThreadPool::WorkerDispatcher::dispatch(TASKINFO *newTask)
{
	bool noFreeWorkers = true;
	long oldState = 0;
	TASKINFO emptyTask = {0};

	for (int i = 0; i < threads.size(); i++)
	{
		if (WaitForSingleObject(threads[i]->mutexBusy, 0) == WAIT_OBJECT_0)
		{
			if (noFreeWorkers)
			{
				if (threads[i]->currentState == CAN_WORK || threads[i]->currentState == WILL_DIE )
				{
					threads[i]->task = *newTask;
					ReleaseSemaphore(threads[i]->semaphore, 1, &oldState);
					noFreeWorkers = false;
				}
			}
			else
			{
				if (threads[i]->currentState == WILL_DIE)
				{
					threads[i]->task = emptyTask;
					ReleaseSemaphore(threads[i]->semaphore, 1, &oldState);
				}
			}
			ReleaseMutex(threads[i]->mutexBusy);
		}
	}
	if (noFreeWorkers)
		createWorker(newTask);
}

void ThreadPool::WorkerDispatcher::createWorker(TASKINFO *newTask)
{
	//WORKER tmp = { 0 };

	threads.push_back(new WORKER);

	threads.back()->mutexBusy = CreateMutex(NULL, false, NULL);

	WaitForSingleObject(threads.back()->mutexBusy, INFINITE); //LOCK

	threads.back()->task.userFunction = newTask->userFunction;
	threads.back()->task.param = newTask->param;
	threads.back()->currentState = CAN_WORK;
	threads.back()->semaphore = CreateSemaphore( NULL , 0, 1000, NULL);
	threads.back()->thread = CreateThread(NULL, 0, ThreadPool::WorkerDispatcher::threadWorker, threads.back(), 0, NULL);
    //LOG(INFO) << "id "<< GetCurrentThreadId() << ": create thread: id = " << GetThreadId(threads.back()->thread);
	
	ReleaseMutex(threads.back()->mutexBusy);   //UNLOCK

	if (threads.size() > parent->NthreadMax)
        ;
            //LOG(WARNING) << "WARNING! EXCEEDED THE MAX N OF THREADS!";
}

void ThreadPool::WorkerDispatcher::killWorker(WORKER *target)
{
    //LOG(INFO) << "id "<< GetCurrentThreadId() << ": kill thread: id = " << GetThreadId(target->thread);
	CloseHandle(target->mutexBusy);
	CloseHandle(target->semaphore);
	CloseHandle(target->thread);
}

DWORD WINAPI ThreadPool::WorkerDispatcher::threadWorkerDispatcher(PVOID pvParam)
{
	ThreadPool::WorkerDispatcher *me = (ThreadPool::WorkerDispatcher *)pvParam;
	ThreadPool *parent = me->parent;
	TASKINFO currentTask;

    //LOG(INFO) << "id "<< GetCurrentThreadId() << ": worker dispatcher thread created";

	while (true)
	{
		WaitForSingleObject(parent->notEmptySemaphore, INFINITE);
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
		WaitForSingleObject(me->mutexBusy, INFINITE);
		State instruction = me->currentState;  //save instruction from WD

		me->currentState = BUSY;
		FuncType F = me->task.userFunction;
		void * param = me->task.param;	
		if (F != NULL)	
			F(param);

		if (instruction == WILL_DIE)
		{
			me->currentState = DEAD;
			ReleaseMutex(me->mutexBusy);

			return 0;
		}

		me->currentState = CAN_WORK;
		ReleaseMutex(me->mutexBusy);
		WaitForSingleObject(me->semaphore, INFINITE);   //wait for next task
	}

	return 0;
}

