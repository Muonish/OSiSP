#pragma once
#include <algorithm>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
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
	void AddThread(void);
	void AddLog(string s);
public:
	HANDLE mutex;
	HANDLE mutexLog;
	vector<HANDLE> threads;
	queue<FuncType> tasks;
	queue<void*> parameters;
	ofstream logFile;
	int Nthread;
	int threadCounter;
	static DWORD WINAPI  ThreadFunction(PVOID pvParam);
};
