// SharedMemory.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>

#define BUF_SIZE 256

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	LPCWSTR appName = L"C:\\Users\\Lili\\Documents\\Visual Studio 2012\\Projects\\OSiSP\\SharedMemory\\Debug\\ProcessApp.exe";
	int mainID = GetCurrentProcessId();
	int Nprocesses = 0;
	HANDLE mutex;
	HANDLE hMapping;
	char *lpData;
	char lpMess[BUF_SIZE] = { 0 };
	string lpBuffer = "Message from main process id = " + to_string(GetCurrentProcessId());
	STARTUPINFO si;
	vector<PROCESS_INFORMATION *> pi;

	hMapping = CreateFileMapping( INVALID_HANDLE_VALUE,
								  NULL, PAGE_READWRITE, 0, BUF_SIZE, L"ShareMemory");
	if (hMapping == NULL) return 1;

	lpData = (char *) MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	if (lpData == NULL) return 1;

	mutex = CreateMutex(NULL, false, L"MutexShareMemory");

	cout << "Enter the number of processes:" << endl; 
	cin >> Nprocesses;
	if (Nprocesses > 0)
	{
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);

		pi.reserve(Nprocesses); //allocate memory to vector
		for (int i = 0; i < Nprocesses; i++)
		{
			pi.push_back(new PROCESS_INFORMATION);
			ZeroMemory(pi.back(), sizeof(PROCESS_INFORMATION));

			if( !CreateProcess(appName, NULL, NULL, NULL, FALSE,
							   NULL, NULL, NULL, &si, pi.back()))
			{
				cout << "Could not create " << i << " process!" << endl;
				return 1;
			}
			cout << "process " << i << " created" << endl;
		}

		for (int i = 0; i < 1; i++)
		{
			WaitForSingleObject(mutex, INFINITE);
			strcpy_s(lpMess, lpBuffer.c_str());
			CopyMemory(lpData, lpMess, BUF_SIZE);
			cout << "Main process write: " << lpMess << endl;

			ReleaseMutex(mutex);
		}

	}

	_getch();

	WaitForSingleObject(mutex, INFINITE);
	lpBuffer = "END";
	strcpy_s(lpMess, lpBuffer.c_str());
	CopyMemory(lpData , lpMess, BUF_SIZE);
	ReleaseMutex(mutex);

	_getch();

	UnmapViewOfFile(lpData);
	CloseHandle(hMapping);

	// Close process and thread handles.
	for (int i = 0; i < Nprocesses; i++)
	{
		CloseHandle( pi[i]->hProcess );
		CloseHandle( pi[i]->hThread );
	}

	return 0;
}

