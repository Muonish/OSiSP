// MyLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define ContactsDLL
#include "MPool.h"
#include "ContactsDLL.h"
#include "string.h"
#include "stdlib.h"
#include "algorithm"
#include "vector"

using namespace std;

ContactsDLL void* LoadBase(LPCWSTR BaseName)
{
	HANDLE mapoffile;
	HANDLE file;
	if( (mapoffile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,  BaseName)) == NULL )
	{
		file = CreateFile(BaseName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 

		mapoffile = CreateFileMapping(file,  NULL, PAGE_READWRITE, 0, 100, BaseName);
		printf("This process create map of file\n");
	}
	else
	{
		printf("This process open map of file\n");
	}
	return MapViewOfFile( mapoffile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
}

void ThreadSearch( void* parameters);
void printRecordDetails(int propertyIndex, char* propertyValue);
void setRecordProperty(int propertyIndex, ContactRecord *currentRecord, char* sCount);

ContactsDLL int* ParallelSearch(int itemNumber, char* itemValue, void* base)
{
	HANDLE HITEM = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 100, L"MYITEM");
	char* ITEM = (char*)MapViewOfFile( HITEM, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	strcpy(ITEM, itemValue);
	
	char*sBase = (char*)base;
	int position = 0;

	while (sBase[position] != '\n') // ��������� ���-�� ������� � ����
	{
		position++;
	}

	char sCount[100];
	strncpy( sCount, sBase, position );
	position++;
	sBase += position;  // ������� ��������� �� ����� ����, ������� �������� ����� �������, ���� �� ��������� ���

	int totalRecordsCount = atoi(sCount);	//� ������ ���� �������� ���������� ���������
	
	HANDLE readPipes[10], writePipes[10];
	HANDLE readPipe, writePipe;
	MPool threadPool;

	for (int i = 0; i < 10; i++)
	{
		CreatePipe( &readPipe, &writePipes[i], NULL, 0);  // ������� ����� ���������� ��������� ��� ��������� ������� � writePipes[]
		CreatePipe( &readPipes[i], &writePipe,  NULL, 0);  // ������� ����� ��������� ���������� ������ ��������� ������� � readPipes[]

		PHANDLE pipes = (HANDLE*)malloc(sizeof(HANDLE) * 2);
		pipes[0] = writePipe;  // ��� ������ ����������� ���������� ������
		pipes[1] = readPipe;  // ��� �������� ���������� � �����
		
		threadPool.AddTask(ThreadSearch, pipes);
		//CreateThread(NULL, 0, ThreadSearch, pipes, 0, NULL);
	}

	DWORD buf;
	int threadItemsCount = totalRecordsCount / 10;  // ������� ������ ���������� ������ ���-�� ������� � ����

	if ((threadItemsCount < 1) && (totalRecordsCount > 0))
	{
		threadItemsCount = 1;
	}	

	int tempCount = totalRecordsCount;
	position = 0;

	for( int i = 0; i < 10; i++ )
	{
		WriteFile( writePipes[i], &itemNumber,sizeof(itemNumber),&buf, NULL);  
		WriteFile( writePipes[i], &ITEM ,sizeof(ITEM),&buf, NULL);
		int startScanPos;

		if (totalRecordsCount >= threadItemsCount)
		{
			startScanPos = threadItemsCount;
		}
		else
		{
			startScanPos = totalRecordsCount;
		}
			
		//tempCount -= startScanPos;
		WriteFile(writePipes[i], &startScanPos, sizeof(startScanPos), &buf, NULL);
		WriteFile( writePipes[i], &sBase, sizeof(sBase), &buf, NULL);
		WriteFile( writePipes[i], &position, sizeof(position), &buf, NULL);

		for (int j = 0; j < startScanPos; j++)  //startScanPos - ����� ������, �� ������� ������ ���� ���������� ���������
		{
			while (*sBase != '\n')   // ������� ���������, ��� ���� ����� ��� ������ �������� ����� �� � ������, � � ����������� �����
			{
				sBase++;
			}
			
			position++;
		}

		sBase++;
	}

	int resultCount = 0;

	for (int i = 0; i < 10; i++)
	{
		ContactRecord record;
		ReadFile( readPipes[i] ,&tempCount, sizeof(int),&buf, NULL);
		resultCount += tempCount;

		for (int j = 0; j < tempCount; j++)  // c������� ���������� ������ �� ������� ������
		{
			ReadFile(readPipes[i], &record, sizeof(record), &buf, NULL);

			printf("\n\t{ID} = %s;\n\t{SurName} = %s,\n\t{FirstName} = %s;\n\t{SubName} = %s;\n\t{Street} = %s;\n\t{House} = %s;\n\t{Flat} = %s;\n\t{Unit} = %s\n\n",
				record.id,
				record.surName,
				record.firstName,
				record.subName,
				record.street,
				record.house,
				record.flat,
				record.unit);
		}
	}

	printf("---------------------------\nFinded records: %d\n", resultCount);
	CloseHandle(HITEM);
	return NULL;	
}

void ThreadSearch(void* parameters)
{
	HANDLE* hparameter = (HANDLE*) parameters;

	int itemNumber;
	int checkingItemCount;
	char* item;

	DWORD buf;
	char* sBase;
	int startPosToSearch;   // hparameter[0] - writePipe;  hparameter[1] - readPipe;

	ReadFile((hparameter[1]) ,&itemNumber,sizeof(itemNumber),&buf, NULL);
	ReadFile((hparameter[1]), &item ,sizeof(&item),&buf, NULL);
	ReadFile((hparameter[1]), &checkingItemCount ,sizeof(checkingItemCount), &buf, NULL);
	ReadFile((hparameter[1]), &sBase ,sizeof(sBase), &buf, NULL);
	ReadFile(hparameter[1], &startPosToSearch, sizeof(startPosToSearch), &buf, NULL);

	vector<int> result;
	int position = 0;
	char currentPropertyValue[100];

	vector<ContactRecord> resultRecords;
	ContactRecord currentRecord;
	 
	for( int i = 0; i < checkingItemCount; i++) 
	{

		bool isRecordTarget = false;

		for (int j = 0; j < 8; j++ )  // ������ ������� ������� ������
		{
			int startPosition = position;

			while (sBase[position] != ';')
			{
				position++;
			}

			strncpy(currentPropertyValue, sBase + startPosition, position - startPosition);
			currentPropertyValue[position - startPosition] = '\0';

			setRecordProperty(j, &currentRecord, currentPropertyValue);

			if (j == itemNumber)
			{
				if (strcmp(currentPropertyValue, item) == 0 )
				{
					isRecordTarget = true;
					result.push_back(i);
					printf("\nThread %d found %d\n------------------------\n", GetCurrentThreadId(), i + startPosToSearch);
				}
			}

			position++;
		}

		if (isRecordTarget)
		{
			/*
			printf("\n\t{ID} = %s;\n\t{FirstName} = %s,\n\t{SurName} = %s;\n\t{SubName} = %s;\n\t{Street} = %s;\n\t{House} = %s;\n\t;{Flat} = %s;\n\t{Unit} = %s\n\n", 
				currentRecord.id,
				currentRecord.firstName,
				currentRecord.surName,
				currentRecord.subName,
				currentRecord.street,
				currentRecord.house,
				currentRecord.flat,
				currentRecord.unit);
				*/

			resultRecords.push_back(currentRecord);
		}

		position++;
	}

	int resultCount = result.size();
	WriteFile(hparameter[0], &resultCount, sizeof(resultCount), &buf, NULL);

	for (int j = 0; j < result.size(); j++)
	{
		//WriteFile(hparameter[0], &result[j], sizeof(int), &buf, NULL);
		WriteFile(hparameter[0], &resultRecords[j], sizeof(ContactRecord), &buf, NULL);
	}
}



void setRecordProperty(int propertyIndex, ContactRecord *currentRecord, char* sCount)
{
	switch (propertyIndex)
	{
	case 0:
		strcpy(currentRecord->id, sCount);
		break;
	case 1:
		strcpy(currentRecord->surName, sCount);
		break;
	case 2:
		strcpy(currentRecord->firstName, sCount);
		break;
	case 3:
		strcpy(currentRecord->subName, sCount);
		break;
	case 4:
		strcpy(currentRecord->street, sCount);
		break;
	case 5:
		strcpy(currentRecord->house, sCount);
		break;
	case 6:
		strcpy(currentRecord->flat, sCount);
		break;
	case 7:
		strcpy(currentRecord->unit, sCount);
		break;
	default:
		break;
	}
}