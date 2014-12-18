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

enum State { WILL_DIE, BUSY, CAN_WORK, DEAD };
typedef void (*FuncType)(void*);

struct TASKINFO
{
	FuncType userFunction;
	void *param;
};

class ThreadPool
{
private:
	class TaskDispatcher
	{
	private:
		ThreadPool *parent;
	public:
		TaskDispatcher(ThreadPool *);
		~TaskDispatcher(void);

		bool add(FuncType, void*);
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
			TASKINFO task;
			State currentState;
			HANDLE semaphore;
			HANDLE mutexBusy;
		};

		TASKINFO requestTask();
		void lookAroundWorkers();
		void dispatch(TASKINFO *);
		void createWorker(TASKINFO *);
		void killWorker(WORKER *);

		static DWORD WINAPI threadWorkerDispatcher(PVOID pvParam);
		static DWORD WINAPI threadWorker(PVOID pvParam);

		vector<WORKER*> threads;
		int threadCounter;
		ThreadPool *parent;
		HANDLE threadMain;


	};

	int NthreadMin;
	int NthreadMax;
	queue<TASKINFO> tasks;
	HANDLE notEmptySemaphore;
	HANDLE queueMutex;

	//static DWORD WINAPI threadFunction(PVOID pvParam);

public:
	ThreadPool(int, int);
	~ThreadPool(void);
	TaskDispatcher *taskd;
	WorkerDispatcher *workd;
};
