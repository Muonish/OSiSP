#include "stdafx.h"
#ifdef ContactsDLL
#define ContactsDLL __declspec(dllexport) 
#else
#define ContactsDLL __declspec(dllimport) 
#endif

ContactsDLL void* LoadBase(LPCWSTR BaseName);
ContactsDLL int* ParallelSearch(int itemNumber, char* item, void* base);


struct ContactRecord {
	char id[100];
	char firstName[100];
	char surName[100];
	char subName[100];
	char street[100];
	char house[100];
	char flat[100];
	char unit[100];
};