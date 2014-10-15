#pragma once
#include <algorithm>
#include <queue>
#include "windows.h"
#include "process.h"

using namespace std;
typedef void (*FuncType)(void*);
class ThreadPool
{
public:
	ThreadPool(int);
	~ThreadPool(void);
	long AddTask(FuncType, void*);
public:
	HANDLE mutex;
	HANDLE *threads;
	queue<FuncType> tasks;
	queue<void*> parameters;
	static DWORD WINAPI  ThreadFunction(PVOID pvParam);
};
