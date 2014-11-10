// SharedMemory.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	int mainID = GetCurrentProcessId();
	HANDLE hMapping;
	char* lpData;
	char* lpBuffer = "I'm data.";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL, L"", NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
		return 0;

	if (GetCurrentProcessId() == mainID)
	{
		hMapping = CreateFileMapping( (HANDLE)(-1), // page file
		NULL, PAGE_READWRITE, 0, 0x0100, L"MyShare");
		if (hMapping == NULL) exit(0);

		// lpData get adress of location
		lpData = (char*) MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS,0,0,0);
		if (lpData == NULL) exit(0);

		memcpy (lpData , lpBuffer, sizeof(*lpBuffer));
	}
	else 
	{

	}

	UnmapViewOfFile(lpData);
	CloseHandle(hMapping);
	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return 0;
}

