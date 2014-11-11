// ProcessApp.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <string>

#define BUF_SIZE 256

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE mutex;
	HANDLE hMapping;
	char * lpData;
	char lpMess[BUF_SIZE] = { 0 };
	string lpBuffer = "Message from child process id = " + to_string(GetCurrentProcessId());

	hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,   // read/write access
							   FALSE,                 // do not inherit the name
							   L"ShareMemory");       // name of mapping object
	if (hMapping == NULL)
	{
		cout << "Could not open file mapping object!" << endl;
		return 1;
	}

	lpData = (char *)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS,0,0,BUF_SIZE);
	if (lpData == NULL) return 1;
	
	mutex = OpenMutex(MUTEX_ALL_ACCESS, true, L"MutexShareMemory");
	if (mutex == NULL) return 1;
	
	Sleep(100);
	while (true)
	{
		WaitForSingleObject(mutex, INFINITE);
		
		/* read */
		CopyMemory(lpMess, lpData , BUF_SIZE);
		cout << "Child process id=" << GetCurrentProcessId() << "  read: " << lpMess << endl;
		if (!strcmp(lpMess, "END")) break;

		/* write */
		strcpy_s(lpMess, lpBuffer.c_str());
		CopyMemory(lpData , lpMess, BUF_SIZE);
		cout << "Child process id=" << GetCurrentProcessId() << " write: " << lpMess << endl;

		Sleep(300);

		ReleaseMutex(mutex);
	}

	UnmapViewOfFile(lpData);
	CloseHandle(hMapping);

	return 0;
}

