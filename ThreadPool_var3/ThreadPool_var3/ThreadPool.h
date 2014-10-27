#pragma once
#include <algorithm>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include "windows.h"
#include "process.h"
#define _ELPP_THREAD_SAFE
#include "easylogging++.h"

using namespace std;

enum State { will_die, busy, can_work, dead };
typedef void (*FuncType)(void*);

struct TASKINFO
{
	FuncType task;
	void *param;
};

class ThreadPool
{
private:
	class TaskDispatcher
	{
	private:
		ThreadPool *parent;
		HANDLE addMutex;
		queue<TASKINFO> tasks;
	public:
		TaskDispatcher(ThreadPool *);
		~TaskDispatcher(void);

		bool add(FuncType, void*);
		TASKINFO getTask();

	};

	class WorkerDispatcher
	{
	public:
		WorkerDispatcher(ThreadPool *);
		~WorkerDispatcher(void);
	private:
		struct WORKER
		{
			HANDLE thread;
			FuncType task;
			State currentState;
			HANDLE mutex;
		};
		vector<WORKER> threads;
		int threadCounter;
		ThreadPool *parent;
		HANDLE threadMain;


		static DWORD WINAPI threadWorkerDispatcher(PVOID pvParam);
		static DWORD WINAPI threadWorker(PVOID pvParam);
	};

	int NthreadMin;
	int NthreadMax;
	HANDLE requestMutex;

	//static DWORD WINAPI threadFunction(PVOID pvParam);

public:
	ThreadPool(int, int);
	~ThreadPool(void);
	TaskDispatcher *task;
};
