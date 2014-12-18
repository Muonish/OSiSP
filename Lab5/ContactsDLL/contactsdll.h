#ifndef CONTACTSDLL_H
#define CONTACTSDLL_H

#include "contactsdll_global.h"
#include "windows.h"
#include "string"

CONTACTSDLLSHARED_EXPORT void* LoadBase(wchar_t * BaseName);
CONTACTSDLLSHARED_EXPORT std::string ParallelSearch(int itemNumber, char* item, void* base);


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

#endif // CONTACTSDLL_H
