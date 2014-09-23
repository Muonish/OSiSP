// Win32Lab1_new.cpp
// Lili 09.2014

#include "stdafx.h"
#include "Win32Lab1_new.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <math.h>

#define PI 3.14159265358979323846


#define MAX_LOADSTRING 100

// Global variables
HINSTANCE hInst;								// current instance
static TCHAR szTitle[] = _T("Win32 Paint Application");
static TCHAR szWindowClass[] = _T("win32app");			// The main window class name.
static int currentFigure = IDM_PEN;
BOOL fTracking = FALSE;
BOOL fPolyline = FALSE;
POINTS ptsBegin;
HMENU hMenu;
HDC hdc;
HDC memDC;
HDC memDC2;
HBITMAP memBM;
HBITMAP memBM2;
RECT lprect;
HBRUSH Brush = CreateSolidBrush(RGB(255,255,255));
HGDIOBJ hOldBush;

// Functions included in this module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
int MouseMoveAction(HWND, LPARAM);
int CommandCase(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int Polygon3(POINTS);
int Polygon4(POINTS);
int Polygon5(POINTS);
int Polygon6(POINTS);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialization of global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32LAB1_NEW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Initialize the application:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32LAB1_NEW));

    // Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: registeres the window class
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32LAB1_NEW));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32LAB1_NEW);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: saves processing instance and create main window
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, 
					   szTitle, 
					   WS_OVERLAPPEDWINDOW,
					   CW_USEDEFAULT, 
					   0, 
					   CW_USEDEFAULT, 
					   0, 
					   NULL, 
					   NULL, 
					   hInstance, 
					   NULL);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		hMenu = GetMenu(hWnd);
        hdc = GetDC(hWnd);					// retrieves a handle to a device context (DC) for the client area
		memDC = CreateCompatibleDC(hdc);
		memDC2 = CreateCompatibleDC(hdc);
		GetClientRect(hWnd, &lprect);
		memBM = CreateCompatibleBitmap(hdc, lprect.right, lprect.bottom);
		memBM2 = CreateCompatibleBitmap(hdc, lprect.right, lprect.bottom);
		SelectObject ( memDC, memBM);
		SelectObject ( memDC2, memBM2);
		FillRect(memDC,&lprect, Brush);
		FillRect(memDC2,&lprect, Brush);
		ps.hdc = hdc;
		ps.rcPaint = lprect;
		ps.fErase = true;
        break;
	case WM_COMMAND:
		CommandCase(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);						// capture the mouse
		fTracking = TRUE;
		if (!fPolyline) ptsBegin = MAKEPOINTS(lParam);			// get the begin coords in POINTS format
		break;
	case WM_MOUSEMOVE:
		if (fTracking)
		{
			MouseMoveAction(hWnd, lParam);
		}
		break;
	case WM_LBUTTONUP:
		BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
        fTracking = FALSE;
		if (fPolyline)
			ptsBegin = MAKEPOINTS(lParam);
        ClipCursor(NULL);						// free cursor
        ReleaseCapture();
		InvalidateRect(hWnd,&lprect,false);
        return 0;
	case WM_RBUTTONUP:
		fPolyline = FALSE;
		BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		InvalidateRect(hWnd,&lprect,false);
		break;
	case WM_DESTROY:
		ReleaseDC(hWnd, hdc);				// free DC
		ReleaseDC(hWnd, memDC);				// free memDC
		ReleaseDC(hWnd, memDC2);				// free memDC2
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for "About ..."
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
int MouseMoveAction(HWND hWnd, LPARAM lParam)
{
    POINTS ptsEnd;

	ptsEnd = MAKEPOINTS(lParam);		// get the end coords in POINTS format
	MoveToEx(memDC2, ptsBegin.x, ptsBegin.y, (LPPOINT) NULL);
	Brush = ( HBRUSH ) GetStockObject( HOLLOW_BRUSH );
	hOldBush = SelectObject( memDC2, Brush );

	switch (currentFigure)
	{
	case IDM_PEN:
		LineTo(memDC2, ptsEnd.x, ptsEnd.y);
		BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		ptsBegin.x = ptsEnd.x;
		ptsBegin.y = ptsEnd.y;
		break;
	case IDM_POLYLINE:
		fPolyline = TRUE;
	case IDM_LINE:
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		MoveToEx(memDC2, ptsBegin.x, ptsBegin.y, (LPPOINT) NULL);
		LineTo(memDC2, ptsEnd.x, ptsEnd.y);
		break;
	case IDM_ELLIPSE:
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		Ellipse(memDC2, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
		break;
	case IDM_RECTANGLE:
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		Rectangle(memDC2, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
		break;
	case IDM_P3:
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		Polygon3(ptsEnd);
		break;
	case IDM_P4:
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		Polygon4(ptsEnd);
		break;
	case IDM_P5:
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		Polygon5(ptsEnd);
		break;
	case IDM_P6:
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		Polygon6(ptsEnd);
		break;
	case IDM_CLEAR:
		Brush = ( HBRUSH ) GetStockObject( WHITE_BRUSH );
		hOldBush = SelectObject( memDC2, Brush );
		FillRect(memDC2, &lprect, Brush);
		FillRect(memDC, &lprect, Brush);
		BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		break;
	}
	InvalidateRect(hWnd,&lprect,false);
	UpdateWindow(hWnd);

	return 0;
}

int CommandCase(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	// Menu:
	currentFigure = wmId;
	switch (wmId)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	case IDM_PEN:
	case IDM_TEXT:
			/*CheckMenuRadioItem(hMenu, IDM_PEN, IDM_TEXT, wmId, MF_BYCOMMAND);
			break;*/
	case IDM_LINE:
	case IDM_POLYLINE:
	case IDM_ELLIPSE:
	case IDM_RECTANGLE:
		/*CheckMenuRadioItem(hMenu, IDM_LINE, IDM_RECTANGLE, wmId, MF_BYCOMMAND);
		break;*/
	case IDM_P3:
	case IDM_P4:
	case IDM_P5:
	case IDM_P6:
		/*CheckMenuRadioItem(hMenu, IDM_P3, IDM_P6, wmId, MF_BYCOMMAND);
		break;*/
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	//CheckMenuRadioItem(hMenu, IDM_LINE, IDM_RECTANGLE, wmId, MF_BYCOMMAND);
}
int Polygon3(POINTS ptsEnd)
{
	MoveToEx(memDC2, ptsBegin.x + ((ptsEnd.x - ptsBegin.x)/2), ptsBegin.y, (LPPOINT) NULL);
	LineTo(memDC2, ptsEnd.x, ptsEnd.y);
	LineTo(memDC2, ptsBegin.x, ptsEnd.y);
	LineTo(memDC2, ptsBegin.x + ((ptsEnd.x - ptsBegin.x)/2), ptsBegin.y);

	return 0;
}
int Polygon4(POINTS ptsEnd)
{
	int deltaX = ((ptsEnd.x - ptsBegin.x)/2);
	int deltaY = ((ptsEnd.y - ptsBegin.y)/2);
	MoveToEx(memDC2, ptsBegin.x + deltaX, ptsBegin.y, (LPPOINT) NULL);
	LineTo(memDC2, ptsEnd.x, ptsBegin.y + deltaY);
	LineTo(memDC2, ptsBegin.x + deltaX, ptsEnd.y);
	LineTo(memDC2, ptsBegin.x, ptsBegin.y + deltaY);
	LineTo(memDC2, ptsBegin.x + deltaX, ptsBegin.y);

	return 0;
}
int Polygon5(POINTS ptsEnd)
{
	int newY = ptsBegin.y + (ptsEnd.y - ptsBegin.y)*tan(PI/5)/2;
	int rightX = ptsEnd.x - (ptsEnd.x - ptsBegin.x)*tan(PI/10)*tan(PI/2.5)/(1/sin(PI/5) + 2); 
	int leftX = ptsBegin.x + ptsEnd.x - rightX;

	MoveToEx(memDC2, ptsBegin.x + ((ptsEnd.x - ptsBegin.x)/2), ptsBegin.y, (LPPOINT) NULL);
	LineTo(memDC2, ptsEnd.x, newY);
	LineTo(memDC2, rightX, ptsEnd.y);
	LineTo(memDC2, leftX, ptsEnd.y);
	LineTo(memDC2, ptsBegin.x, newY);
	LineTo(memDC2, ptsBegin.x + ((ptsEnd.x - ptsBegin.x)/2), ptsBegin.y);

	return 0;
}
int Polygon6(POINTS ptsEnd)
{
	int newY = ptsBegin.y + (ptsEnd.y - ptsBegin.y)/2;
	int leftX = ptsBegin.x + (ptsEnd.x - ptsBegin.x)*tan(PI/6)*tan(PI/3)/4; 
	int rightX = ptsEnd.x - (leftX - ptsBegin.x);

	MoveToEx(memDC2, ptsBegin.x, newY, (LPPOINT) NULL);
	LineTo(memDC2, leftX, ptsBegin.y);
	LineTo(memDC2, rightX, ptsBegin.y);
	LineTo(memDC2, ptsEnd.x, newY);
	LineTo(memDC2, rightX, ptsEnd.y);
	LineTo(memDC2, leftX, ptsEnd.y);
	LineTo(memDC2, ptsBegin.x, newY);

	return 0;
}