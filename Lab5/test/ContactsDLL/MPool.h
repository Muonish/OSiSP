#pragma once
#include <algorithm>
#include <queue>
#include "windows.h"
#include "process.h"
using namespace std;
typedef void (*FuncType)(void*);
class MPool
{
public:
	MPool(void);
	~MPool(void);
	long AddTask(FuncType, void*);
public:
	LPDWORD IDThreads[10];
	HANDLE semaphore;
	HANDLE mutex;
	HANDLE threads[10];
	queue<FuncType> tasks;
	queue<void*> parameters;
	static DWORD WINAPI  ThreadFunction(PVOID pvParam);
};