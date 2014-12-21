
// lr4_v3.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// Clr4_v3App:
// О реализации данного класса см. lr4_v3.cpp
//

class Clr4_v3App : public CWinApp
{
public:
	Clr4_v3App();

// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern Clr4_v3App theApp;