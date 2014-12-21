// IndexDll.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include <vector>
#include <stdio.h>

const int t = 6;
typedef struct Abonent
{
	char secondName[20];
	char initials[5];
	char number[15];
	char adress[20];
};

typedef struct TKey
{
	char key[255];
	std::vector<Abonent*> abonentsList;
};

typedef struct TNode
{
	int keyNumber;
	bool isLeaf;
	TKey keyArray[2 * t - 1];
	TNode* childrenArray[2*t];
};

typedef struct TT
{
	TNode* root;
};


TT* secondNameBTree = new TT;
TT* adressBTree = new TT;
TT* numberBTree = new TT;
void CreateBTree(TT* T);
void BTreeInsert(TT *T, TKey* k);
int BTreeDelete(TNode* x, char* key);
TNode* BTreeSearch(TNode* x, char* key, int* resultIndex);
void DataBaseInsert(char secondName[20], char initials[5], char number[15], char adress[20]);

extern "C"__declspec(dllexport) void __stdcall CreateIndex()
{
	CreateBTree(secondNameBTree);
	CreateBTree(adressBTree);
	CreateBTree(numberBTree);
}

extern "C"__declspec(dllexport) void __stdcall LoadPhoneBase()
{
	FILE *phoneBaseFile;
	phoneBaseFile = fopen("C:\\Users\\Lili\\Documents\\Visual Studio 2012\\Projects\\OSiSP\\Serkova_Breshko_lr4_v3\\lr4_v3\\Debug\\PhoneDataBase.txt", "rt");
	char currentAbonent[55];
	char secondName[20];
	char initials[5];
	char number[15];
	char adress[40];
	while (fgets(currentAbonent, 55, phoneBaseFile) != 0)
	{
		int i = 0;
		while (currentAbonent[i] != ';')
		{
			secondName[i] = currentAbonent[i];
			i++;
		}
		secondName[i] = '\0';
		i++;
		int j = 0;
		while (currentAbonent[i] != ';')
		{
			initials[j] = currentAbonent[i];
			j++;
			i++;
		}
		initials[j] = '\0';
		i++;
		j = 0;
		for (int k = 0; k < 3; k++)
		{

			while (currentAbonent[i] != ';')
			{
				adress[j] = currentAbonent[i];
				i++;
				j++;
			}
			adress[j] = ' ';
			j++;
			i++;
		}
		adress[j - 1] = '\0';
		j = 0;
		while (currentAbonent[i] != '\n')
		{
			number[j] = currentAbonent[i];
			j++;
			i++;
		}
		number[j] = '\0';
		DataBaseInsert(secondName, initials, number, adress);
	}
}

extern "C" __declspec(dllexport) char*** __stdcall DataBaseSearchBySecondName(char secondName[20])
{
	int resultIndex = 1;
	TNode* resultNode = BTreeSearch(secondNameBTree->root, secondName, &resultIndex);
	if (resultIndex != -1)
	{
		std::vector<Abonent*> abonentsList = resultNode->keyArray[resultIndex].abonentsList;
		int vectorSize = abonentsList.size();
		char*** exitArray = new char**[vectorSize];
		for (int i = 0; i < vectorSize; i++)
		{
			char** resultArray = new char*[4];
			resultArray[0] = resultNode->keyArray[resultIndex].abonentsList[i]->secondName;
			resultArray[1] = resultNode->keyArray[resultIndex].abonentsList[i]->initials;
			resultArray[2] = resultNode->keyArray[resultIndex].abonentsList[i]->adress;
			resultArray[3] = resultNode->keyArray[resultIndex].abonentsList[i]->number;
			exitArray[i] = resultArray;
		}
		return exitArray;
	}
	return 0;
}

extern "C" __declspec(dllexport) char*** __stdcall DataBaseSearchByAdress(char adress[40])
{
	int resultIndex = 1;
	TNode* resultNode = BTreeSearch(adressBTree->root, adress, &resultIndex);
	if (resultIndex != -1)
	{
		std::vector<Abonent*> abonentsList = resultNode->keyArray[resultIndex].abonentsList;
		int vectorSize = abonentsList.size();
		char*** exitArray = new char**[vectorSize];
		for (int i = 0; i < vectorSize; i++)
		{
			char** resultArray = new char*[4];
			resultArray[0] = resultNode->keyArray[resultIndex].abonentsList[i]->secondName;
			resultArray[1] = resultNode->keyArray[resultIndex].abonentsList[i]->initials;
			resultArray[2] = resultNode->keyArray[resultIndex].abonentsList[i]->adress;
			resultArray[3] = resultNode->keyArray[resultIndex].abonentsList[i]->number;
			exitArray[i] = resultArray;
		}
		return exitArray;
	}
	return 0;
}

extern "C" __declspec(dllexport) char*** __stdcall DataBaseSearchPhoneNumber(char phoneNumber[20])
{
	int resultIndex = 1;
	TNode* resultNode = BTreeSearch(numberBTree->root, phoneNumber, &resultIndex);
	if (resultIndex != -1)
	{
		std::vector<Abonent*> abonentsList = resultNode->keyArray[resultIndex].abonentsList;
		int vectorSize = abonentsList.size();
		char*** exitArray = new char**[vectorSize];
		for (int i = 0; i < vectorSize; i++)
		{
			char** resultArray = new char*[4];
			resultArray[0] = resultNode->keyArray[resultIndex].abonentsList[i]->secondName;
			resultArray[1] = resultNode->keyArray[resultIndex].abonentsList[i]->initials;
			resultArray[2] = resultNode->keyArray[resultIndex].abonentsList[i]->adress;
			resultArray[3] = resultNode->keyArray[resultIndex].abonentsList[i]->number;
			exitArray[i] = resultArray;
		}
		return exitArray;
	}
	return 0;
}

extern "C" __declspec(dllexport) void __stdcall DataBaseDelete(char secondName[20])
{
	
}

void DataBaseInsert(char secondName[20], char initials[5], char number[15], char adress[40])
{
	Abonent* newAbonent = new Abonent;
	strcpy(newAbonent->secondName, secondName);
	strcpy(newAbonent->initials, initials);
	strcpy(newAbonent->number, number);
	strcpy(newAbonent->adress, adress);
	int resultIndex = 1;
	TNode *resultNode = BTreeSearch(secondNameBTree->root, secondName, &resultIndex);
	if (resultIndex == -1)
	{
		TKey* k = new TKey;
		k->abonentsList.push_back(newAbonent);
		strcpy(k->key, secondName);
		BTreeInsert(secondNameBTree, k);
	}
	else
	{
		resultNode->keyArray[resultIndex].abonentsList.push_back(newAbonent);
	}
	resultNode = BTreeSearch(adressBTree->root, adress, &resultIndex);
	if (resultIndex == -1)
	{
		TKey* k = new TKey;
		k->abonentsList.push_back(newAbonent);
		strcpy(k->key, adress);
		BTreeInsert(adressBTree, k);
	}
	else
	{
		resultNode->keyArray[resultIndex].abonentsList.push_back(newAbonent);
	}
	resultNode = BTreeSearch(numberBTree->root, number, &resultIndex);
	if (resultIndex == -1)
	{
		TKey* k = new TKey;
		k->abonentsList.push_back(newAbonent);
		strcpy(k->key, number);
		BTreeInsert(numberBTree, k);
	}
	else
	{
		resultNode->keyArray[resultIndex].abonentsList.push_back(newAbonent);
	}
}

void CreateBTree(TT* T)
{
	TNode* newNode = new TNode;
	newNode->keyNumber = 0;
	newNode->isLeaf = true;
	T->root = newNode;
}

void BTreeSplitChild(TNode* x, int i)
{
	TNode* z = new TNode;
	TNode* y = x->childrenArray[i];
	z->isLeaf = y->isLeaf;
	z->keyNumber = t - 1;
	for (int j = 0; j < t - 1; j++)
		z->keyArray[j] = y->keyArray[j + t];
	if (!y->isLeaf)
	{
		for (int j = 0; j < t; j++)
			z->childrenArray[j] = y->childrenArray[j + t];
	}
	y->keyNumber = t - 1;
	for (int j = x->keyNumber; j >= i + 1; j--)
	{
		x->childrenArray[j + 1] = x->childrenArray[j];
	}
	x->childrenArray[i + 1] = z;
	for (int j = x->keyNumber - 1; j >= i; j--)
	{
		x->keyArray[j + 1] = x->keyArray[j];
	}
	x->keyArray[i] = y->keyArray[t - 1];
	x->keyNumber++;
}

void BTreeInsertNonFull(TNode* x, TKey* k)												
{
	int i = x->keyNumber - 1;
	if (x->isLeaf)
	{
		while (i >= 0 && strcmp(k->key, x->keyArray[i].key) < 0)									
		{
			x->keyArray[i + 1] = x->keyArray[i];
			i--;
		}
		x->keyArray[i+1] = *k;
		x->keyNumber++;
	}
	else
	{
		while (i >= 0 && strcmp(k->key, x->keyArray[i].key) < 0)									
			i--;
		i++;
		if (x->childrenArray[i]->keyNumber == 2 * t - 1)
		{
			BTreeSplitChild(x, i);
			if (strcmp(k->key, x->keyArray[i].key) > 0)
				i++;
		}
		BTreeInsertNonFull(x->childrenArray[i], k);
	}
}

void BTreeInsert(TT *T, TKey* k)
{
	TNode* root = T->root;
	if (root->keyNumber == 2 * t - 1)
	{
		TNode* s = new TNode;
		T->root = s;
		s->isLeaf = false;
		s->keyNumber = 0;
		s->childrenArray[0] = root;
		BTreeSplitChild(s, 0);
		BTreeInsertNonFull(s, k);
	}
	else
		BTreeInsertNonFull(root, k);
}

TNode* BTreeSearch(TNode* x, char* key, int* resultIndex)
{
	int i = 0;
	while (i <= x->keyNumber - 1 && strcmp(key, x->keyArray[i].key) > 0)
		i++;
	if (i < x->keyNumber && strcmp(key,x->keyArray[i].key)==0)
	{

		*resultIndex = i;
		return x;
	}
	else
	{
		if (x->isLeaf == true)
		{
			*resultIndex = -1;
			return 0;
		}
		else
			BTreeSearch(x->childrenArray[i], key, resultIndex);
	}
}

int BTreeDelete(TNode* x, char* key)
{
	int i = x->keyNumber - 1;
	while (i >= 0 && strcmp(key, x->keyArray[i].key) < 0)
		i--;
	if (x->isLeaf == true)
	{
		if (i >= 0 && strcmp(key, x->keyArray[i].key) == 0)
		{
			while (i < x->keyNumber - 1)
			{
				x->keyArray[i] = x->keyArray[i + 1];
				i++;
			}
			x->keyNumber--;
		}
		else
			return 0;
	}
	else
	{
		if (i >= 0 && strcmp(key, x->keyArray[i].key) == 0)
		{
			TNode* y = x->childrenArray[i];
			TNode* z = x->childrenArray[i + 1];
			if (x->childrenArray[i]->keyNumber > t - 1)
			{
				x->keyArray[i] = y->keyArray[y->keyNumber - 1];
				BTreeDelete(y, y->keyArray[y->keyNumber - 1].key);
			}
			else
			{
				if (z->keyNumber > t - 1)
				{
					x->keyArray[i] = z->keyArray[0];
					BTreeDelete(z, z->keyArray[0].key);
				}
				else
				{
					y->keyArray[y->keyNumber] = x->keyArray[i];
					int j;
					for (j = 0; j < z->keyNumber;j++)
					{
						y->keyArray[y->keyNumber + 1 + j] = z->keyArray[j];
						y->childrenArray[y->keyNumber + 1 + j] = z->childrenArray[j];
					}
					y->childrenArray[y->keyNumber + 2 + j] = z->childrenArray[j + 1];
					y->keyNumber = 2 * t - 1;
					for (int j = i; j < x->keyNumber - 1;j++)
						x->keyArray[j] = x->keyArray[j + 1];
					for (int j = i + 1; j < x->keyNumber; j++)
						x->childrenArray[j] = x->childrenArray[j + 1];
					x->keyNumber--;
					BTreeDelete(y, x->childrenArray[i]->keyArray[t-1].key);
				}
			}
		}
		else
		{
			if (x->childrenArray[i + 1]->keyNumber == t - 1)
			{

				if (i > 0 && x->childrenArray[i]->keyNumber > t - 1)
				{ 
					for (int j = x->childrenArray[i + 1]->keyNumber - 1; j <= 0; j--)
						x->childrenArray[i + 1]->keyArray[j] = x->childrenArray[i + 1]->keyArray[j + 1];
					for (int j = x->childrenArray[i + 1]->keyNumber; j <= 0; j--)
						x->childrenArray[i + 1]->childrenArray[j] = x->childrenArray[i + 1]->childrenArray[j + 1];
					x->childrenArray[i + 1]->keyArray[0] = x->keyArray[i];
					x->childrenArray[i + 1]->childrenArray[0] = x->childrenArray[i]->childrenArray[x->childrenArray[i]->keyNumber];
					x->keyArray[i] = x->childrenArray[i]->keyArray[x->childrenArray[i]->keyNumber - 1];
					x->childrenArray[i]->keyNumber--;
					x->childrenArray[i+1]->keyNumber++;
				}
				else
				{
					if (i < 2*t-1 && x->childrenArray[i + 2]->keyNumber > t - 1)
					{
						x->childrenArray[i + 1]->keyArray[x->childrenArray[i + 1]->keyNumber] = x->keyArray[i+1];
						x->childrenArray[i + 1]->childrenArray[x->childrenArray[i + 1]->keyNumber + 1] = x->childrenArray[i+2]->childrenArray[0];
						x->keyArray[i + 1] = x->childrenArray[i + 2]->keyArray[0];
						for (int j = 0; j < x->childrenArray[i + 2]->keyNumber - 1; j--)
							x->childrenArray[i + 2]->keyArray[j] = x->childrenArray[i + 2]->keyArray[j + 1];
						for (int j = 0; j < x->childrenArray[i + 2]->keyNumber; j--)
							x->childrenArray[i + 2]->childrenArray[j] = x->childrenArray[i + 2]->childrenArray[j + 1];
						x->childrenArray[i + 2]->keyNumber--;
						x->childrenArray[i + 1]->keyNumber++;
					}
					else
					{
						if (i == -1)
							i++;
						TNode* y = x->childrenArray[i];
						TNode* z = x->childrenArray[i + 1];
						y->keyArray[y->keyNumber] = x->keyArray[i];
						int j;
						for (j = 0; j < z->keyNumber; j++)
						{
							y->keyArray[y->keyNumber + 1 + j] = z->keyArray[j];
							y->childrenArray[y->keyNumber + 1 + j] = z->childrenArray[j];
						}
						y->childrenArray[y->keyNumber + 1 + j] = z->childrenArray[j];
						y->keyNumber = 2 * t - 1;
						for (int j = i; j < x->keyNumber - 1; j++)
							x->keyArray[j] = x->keyArray[j + 1];
						for (int j = i + 1; j < x->keyNumber; j++)
							x->childrenArray[j] = x->childrenArray[j + 1];
						x->keyNumber--;
					}
				}
				BTreeDelete(x->childrenArray[i], key);
			}
			else
				BTreeDelete(x->childrenArray[i + 1], key);
		}
	}
	return 1;
}

