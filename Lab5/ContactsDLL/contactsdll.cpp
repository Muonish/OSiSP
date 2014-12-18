#include "contactsdll.h"

#include "ThreadPool.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "conio.h"
#include "algorithm"
#include "vector"

using namespace std;

void* LoadBase(wchar_t * BaseName)
{
    HANDLE mapoffile;
    HANDLE file;
    if( (mapoffile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,  (LPCWSTR)BaseName)) == NULL )
    {
        file = CreateFile((LPCWSTR)BaseName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        mapoffile = CreateFileMapping(file,  NULL, PAGE_READWRITE, 0, 100, (LPCWSTR)BaseName);
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

string ParallelSearch(int itemNumber, char* itemValue, void* base)
{
    HANDLE HITEM = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 100, L"MYITEM");
    char* ITEM = (char*)MapViewOfFile( HITEM, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    strcpy(ITEM, itemValue);

    char *sBase = (char *)base;
    int position = 0;

    while (sBase[position] != '\n') // считываем кол-во записей в базе
    {
        position++;
    }

    char sCount[100];
    strncpy( sCount, sBase, position );
    position++;
    sBase += position;  // смещаем указатель на число байт, которое занимает число записей, дабы не учитывать его

    int totalRecordsCount = atoi(sCount);	//в начале базы хранится количество элементов

    HANDLE readPipes[10], writePipes[10];
    HANDLE readPipe, writePipe;

    ThreadPool *pool = new ThreadPool(10, 10);

    for (int i = 0; i < 10; i++)
    {
        CreatePipe( &readPipe, &writePipes[i], NULL, 0);  // главный поток записывает параметры для поисковых потоков в writePipes[]
        CreatePipe( &readPipes[i], &writePipe,  NULL, 0);  // главный поток считывает результаты работы поисковых потоков с readPipes[]

        PHANDLE pipes = (HANDLE*)malloc(sizeof(HANDLE) * 2);
        pipes[0] = writePipe;  // для записи результатов выполнения потока
        pipes[1] = readPipe;  // для передачи параметров в поток

        pool->taskd->add(ThreadSearch, pipes);
        //CreateThread(NULL, 0, ThreadSearch, pipes, 0, NULL);
    }

    DWORD buf;
    int threadItemsCount = totalRecordsCount / 10;  // каждому потоку выделяется равное кол-во записей в базе

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

        for (int j = 0; j < startScanPos; j++)  //startScanPos - номер строки, на которую должен быть установлен указатель
        {
            while (*sBase != '\n')   // смещаем указатель, для того чтобы все потоки начинали поиск не с начала, а с отведенного места
            {
                sBase++;
            }

            position++;
        }

        sBase++;
    }

    int resultCount = 0;
    string res;

    for (int i = 0; i < 10; i++)
    {
        ContactRecord record;
        ReadFile( readPipes[i] ,&tempCount, sizeof(int),&buf, NULL);
        resultCount += tempCount;

        for (int j = 0; j < tempCount; j++)  // cобираем результаты поиска от каждого потока
        {
            ReadFile(readPipes[i], &record, sizeof(record), &buf, NULL);


            char buf[800];
            sprintf(buf, "\nID = %s;\nSurName = %s,\nFirstName = %s;\nSubName = %s;\nStreet = %s;\nHouse = %s;\nFlat = %s;\nUnit = %s\n\n",
                record.id,
                record.surName,
                record.firstName,
                record.subName,
                record.street,
                record.house,
                record.flat,
                record.unit);
            res += buf;
        }
    }

    //printf("---------------------------\nFinded records: %d\n", resultCount);
    CloseHandle(HITEM);

    delete pool;
    return res;
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

        for (int j = 0; j < 8; j++ )  // анализ свойств текущей записи
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
                    //printf("\nThread %d found %d\n------------------------\n", GetCurrentThreadId(), i + startPosToSearch);
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
