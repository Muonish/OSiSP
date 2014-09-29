// Win32Lab1_new.cpp
// Lili 09.2014

#include "stdafx.h"
#include "Win32Lab1_new.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <atltime.h>
#include <math.h>
#include <commdlg.h>

#define PI 3.14159265358979323846
#define ID_TIMER 120

#define MAX_LOADSTRING 100

// Global variables
HINSTANCE hInst;								// current instance
static TCHAR szTitle[] = _T("Win32 Paint Application");
static TCHAR szWindowClass[] = _T("win32app");			// The main window class name.
static int currentFigure = IDM_PEN;
static int wheelDelta = 0;
static int width = 1;
static WCHAR textSymbols[MAX_LOADSTRING] = {};
POINTS AllocateRect[2];
BOOL fTracking = FALSE;
BOOL fPolyline = FALSE;
BOOL fPrint = FALSE;
BOOL fAllocate = FALSE;
BOOL fPan = FALSE;
int dx, dy;
POINTS ptsBegin, pts;
HMENU hMenu;
HDC hdc, memDC, memDC2, memDCallocate, memDCclock, memDChand;
HBITMAP memBM, memBM2, memBMallocate, memBMclock, memBMhand;
RECT lprect, tmprect;
LOGBRUSH logbrush, oldlogbrush;
HBRUSH Brush;
HPEN Pen = CreatePen(NULL, 1, RGB(0,0,0));
DWORD dColors[3]={255,222,222};
COLORREF bcolor=RGB(0,0,0), pcolor=RGB(0,0,0);

// Functions included in this module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL				GetSaveFile(HWND);
BOOL				GetOpenFile(HWND);
BOOL				PrintFile(HWND, POINTS, POINTS);
BOOL				PenColor(HWND);
BOOL				BrushColor(HWND);
BOOL				PenWidth(int width);
BOOL				InitTimer(HWND);
BOOL				InitDCs(HWND hWnd);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ColorProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int					MouseMoveAction(HWND, LPARAM);
int					TextDrawAction(HWND, WPARAM, LPARAM);
int					CommandCase(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int					Polygon3(POINTS);
int					Polygon4(POINTS);
int					Polygon5(POINTS);
int					Polygon6(POINTS);
int					ClockHandPosition(HWND);

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

   hInst = hInstance; 

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
BOOL GetSaveFile(HWND hWnd)
{

	HDC hdcRef = GetDC(hWnd); 

	int iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE); 
	int iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE); 
	int iWidthPels = GetDeviceCaps(hdcRef, HORZRES); 
	int iHeightPels = GetDeviceCaps(hdcRef, VERTRES); 
 
	RECT rect;

	rect = lprect;
	rect.left = (rect.left * iWidthMM * 100)/iWidthPels; 
	rect.top = (rect.top * iHeightMM * 100)/iHeightPels; 
	rect.right = (rect.right * iWidthMM * 100)/iWidthPels; 
	rect.bottom = (rect.bottom * iHeightMM * 100)/iHeightPels; 

	// Fill the OPENFILENAME structure
	TCHAR szFilters[] = _T("Scribble Files (*.emf)\0*.emf\0\0");
	TCHAR szFilePathName[_MAX_PATH];
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	szFilePathName[0] = '\0';

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName;  // This will hold the file name
	ofn.lpstrDefExt = _T("emf");
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Save File");
	ofn.Flags = OFN_OVERWRITEPROMPT;

	// Open the file save dialog, and choose the file name
	GetSaveFileName(&ofn);

	HDC hdcMeta = CreateEnhMetaFile(NULL, 
          (LPTSTR) ofn.lpstrFile, 
		  &rect, L"LOL\0"/*(LPSTR)szDescription*/); 
	StretchBlt (hdcMeta, 0, 0, lprect.right, lprect.bottom,
		memDC,   0,   0, lprect.right, lprect.bottom, SRCCOPY) ;

	CloseEnhMetaFile(hdcMeta);

	return TRUE;
}

BOOL GetOpenFile(HWND hWnd)
{

	TCHAR szFilters[] = _T("Scribble Files (*.emf)\0*.emf\0\0");
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn = {0}; 

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName;  // This will hold the file name
	ofn.lpstrDefExt = _T("emf");
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Open File");
	ofn.nFileOffset = 0; 
	ofn.nFileExtension = 0; 
	ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
 
	GetOpenFileName(&ofn); 
	HENHMETAFILE hemf = GetEnhMetaFile(ofn.lpstrFile);

	PlayEnhMetaFile(memDC2, hemf, &lprect);  
	PlayEnhMetaFile(memDC, hemf, &lprect);  
	DeleteEnhMetaFile(hemf); 
	InvalidateRect(hWnd,&lprect,false);

	return TRUE;
}
BOOL PrintFile(HWND hWnd, POINTS ptsBegin, POINTS ptsEnd)
{
	PRINTDLG pd;

	ZeroMemory(&pd, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = hWnd;
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;
	pd.Flags = PD_RETURNDC | PD_NOSELECTION | PD_PRINTTOFILE | PD_NOPAGENUMS;
	pd.nMinPage = 1;
	pd.nMaxPage = 1;
	pd.nCopies = 1;

	if (PrintDlg(&pd) == TRUE)
	{
		DOCINFO doc;
		GlobalUnlock(pd.hDevMode);
		DEVNAMES * pdn = (DEVNAMES *)GlobalLock(pd.hDevNames);
		ZeroMemory(&doc, sizeof(doc));
		doc.cbSize = sizeof(doc);
		doc.lpszDatatype = _T("RAW");
		doc.lpszOutput = NULL;
		int lol = StartDoc(pd.hDC, &doc);
		StartPage(pd.hDC);
		StretchBlt(pd.hDC, 0, 0, ptsEnd.x - ptsBegin.x, ptsEnd.y - ptsBegin.y, memDC2, ptsBegin.x, ptsBegin.y,ptsEnd.x - ptsBegin.x,ptsEnd.y - ptsBegin.y, SRCCOPY);
		EndPage(pd.hDC);
		EndDoc(pd.hDC);
		DeleteDC(pd.hDC);
	}
	return TRUE;
}

BOOL PenColor(HWND hWnd)
{
	CHOOSECOLOR cc;

	cc.Flags=CC_RGBINIT|CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hWnd;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = RGB(255,0,0);

	if(ChooseColor(&cc))
		pcolor = (COLORREF)cc.rgbResult;

	Pen = CreatePen(NULL, width, pcolor);
	SelectObject( memDC2, Pen );
	return TRUE;
}
BOOL BrushColor(HWND hWnd)
{
	CHOOSECOLOR cc;

	cc.Flags=CC_RGBINIT|CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hWnd;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = RGB(255,0,0);

	if(ChooseColor(&cc))
		bcolor = (COLORREF)cc.rgbResult;

	logbrush.lbColor = bcolor;
	logbrush.lbStyle = BS_SOLID;
	oldlogbrush = logbrush;
	Brush = CreateBrushIndirect(&logbrush);
	SelectObject( memDC2, Brush );
	return TRUE;
}
BOOL PenWidth(int w)
{
	width = w;
	Pen = CreatePen(NULL, w, pcolor);
	SelectObject( memDC2, Pen );
	return TRUE;
}
BOOL InitTimer(HWND hWnd)
{
	SetTimer(hWnd, ID_TIMER, 500, NULL);
	return TRUE;
}
BOOL InitDCs(HWND hWnd)
{
	hdc = GetDC(hWnd);					// retrieves a handle to a device context (DC) for the client area
	memDC = CreateCompatibleDC(hdc);
	memDC2 = CreateCompatibleDC(hdc);
	memDCallocate = CreateCompatibleDC(hdc);
	memDCclock = CreateCompatibleDC(hdc);
	memDChand = CreateCompatibleDC(hdc);
	GetClientRect(hWnd, &lprect);
	memBM = CreateCompatibleBitmap(hdc, lprect.right, lprect.bottom);
	memBM2 = CreateCompatibleBitmap(hdc, lprect.right, lprect.bottom);
	memBMallocate = CreateCompatibleBitmap(hdc, lprect.right, lprect.bottom); 
	memBMclock = (HBITMAP)LoadImage(NULL, L"imclock.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	memBMhand = CreateCompatibleBitmap(hdc, 300, 300);
	SelectObject (memDC, memBM); 
	SelectObject (memDC2, memBM2);
	SelectObject (memDCallocate, memBMallocate);
	SelectObject (memDCclock, memBMclock);
	SelectObject (memDChand, memBMhand);
	FillRect(memDC,&lprect, Brush);
	FillRect(memDC2,&lprect, Brush);
	FillRect(memDCallocate,&lprect, Brush);
	FillRect(memDChand,&lprect, Brush);
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
		InitDCs(hWnd);
		InitTimer(hWnd);
		logbrush.lbColor = RGB(255,255,255);
		logbrush.lbStyle = BS_HOLLOW;
		logbrush.lbHatch = NULL;
		oldlogbrush = logbrush;
		Brush = CreateBrushIndirect(&logbrush);
		SelectObject( memDC2, Brush );
		SelectObject( memDC2, Pen );
		PenWidth(1);
		ps.hdc = hdc;
		ps.rcPaint = lprect;
		ps.fErase = true;
        break;
	case WM_COMMAND:
		CommandCase(hWnd, message, wParam, lParam);
		break;
	case WM_TIMER:
		ClockHandPosition(hWnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		BitBlt(hdc, lprect.right - 300, 0, lprect.right, lprect.bottom, memDCclock, 0, 0, SRCAND);
		BitBlt(hdc, lprect.right - 300, 0, lprect.right, lprect.bottom, memDChand, 0, 0, SRCAND);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);						// capture the mouse
		fTracking = TRUE;
		memset(textSymbols, 0, MAX_LOADSTRING);
		if (!fPolyline) ptsBegin = MAKEPOINTS(lParam);			// get the begin coords in POINTS format
		if (fPan)
		{
			fAllocate = FALSE;
			if (ptsBegin.x < AllocateRect[1].x && ptsBegin.x > AllocateRect[0].x && ptsBegin.y < AllocateRect[1].y && ptsBegin.y > AllocateRect[0].y) //if in AllocateRect
			{
				BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
				BitBlt(memDCallocate, 0, 0, AllocateRect[1].x - AllocateRect[0].x, AllocateRect[1].y - AllocateRect[0].y, 
					   memDC2, AllocateRect[0].x, AllocateRect[0].y, SRCCOPY);
				logbrush.lbColor = RGB(255, 255, 255);
				logbrush.lbStyle = BS_SOLID;
				Brush = CreateBrushIndirect(&logbrush);
				SelectObject( memDC2, Brush );
				tmprect.left = AllocateRect[0].x;
				tmprect.top = AllocateRect[0].y;
				tmprect.right = AllocateRect[1].x;
				tmprect.bottom = AllocateRect[1].y;
				FillRect(memDC2,&tmprect, Brush);
				FillRect(memDC,&tmprect, Brush);
				dx = ptsBegin.x - AllocateRect[0].x;
				dy = ptsBegin.y - AllocateRect[0].y;
				logbrush.lbStyle = BS_HOLLOW;
				Brush = CreateBrushIndirect(&logbrush);
				SelectObject( memDC2, Brush );
			}
		}
		break;
	case WM_MOUSEMOVE:
		if (fTracking)
		{
			pts = MAKEPOINTS(lParam);
			if (fPrint || fAllocate)
			{
				BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
				Rectangle(memDC2, ptsBegin.x, ptsBegin.y, pts.x, pts.y);
				InvalidateRect(hWnd,&lprect,false);
				UpdateWindow(hWnd);
			}
			else
			{
				if (fPan)
				{
					BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
					BitBlt(memDC2, pts.x - dx, pts.y - dy, AllocateRect[1].x - AllocateRect[0].x, AllocateRect[1].y - AllocateRect[0].y, memDCallocate, 0, 0, SRCCOPY);
					InvalidateRect(hWnd,&lprect,false);
					UpdateWindow(hWnd);
				}
				else
					MouseMoveAction(hWnd, lParam);
			}
		}
		break;
	case WM_LBUTTONUP:
        fTracking = FALSE;
		if (fPrint)
		{
			BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
			PrintFile(hWnd, ptsBegin, MAKEPOINTS(lParam));
			fPrint = FALSE;
			Pen = CreatePen(NULL, width, pcolor);
			SelectObject( memDC2, Pen );
			Brush = CreateBrushIndirect(&oldlogbrush);
			SelectObject( memDC2, Brush );
		}
		if (fPan)
			fPan = FALSE;
		if (fAllocate)
		{
			AllocateRect[0] = ptsBegin;
			AllocateRect[1] =  MAKEPOINTS(lParam);
			fPan = TRUE;
			Pen = CreatePen(NULL, width, pcolor);
			SelectObject( memDC2, Pen );
			Brush = CreateBrushIndirect(&oldlogbrush);
			SelectObject( memDC2, Brush );
		}
		if (fPolyline)
			ptsBegin = MAKEPOINTS(lParam);
		if (!fAllocate) 
			BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
        ClipCursor(NULL);						// free cursor
        ReleaseCapture();
		InvalidateRect(hWnd,&lprect,false);
        return 0;
	case WM_RBUTTONUP:
		fPolyline = FALSE;
		BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		InvalidateRect(hWnd,&lprect,false);
		return 0;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			wheelDelta += 10;
		else 
			wheelDelta -= 10;
		logbrush.lbColor = RGB(255, 255, 255);
		logbrush.lbStyle = BS_SOLID;
		Brush = CreateBrushIndirect(&logbrush);
		SelectObject( memDC2, Brush );
		FillRect(memDC2,&lprect, Brush);
		StretchBlt(memDC2, 0, 0, lprect.right + wheelDelta , lprect.bottom + lprect.bottom*wheelDelta/lprect.right, memDC, 0, 0, lprect.right, lprect.bottom, SRCCOPY);
		InvalidateRect(hWnd,&lprect,false);
		break;
	case WM_CHAR:
		{	
			if (currentFigure == IDM_TEXT)
			{
				TextDrawAction(hWnd, wParam, lParam);
			}
		}
		break;
	case WM_DESTROY:
		DeleteObject(Brush);
		ReleaseDC(hWnd, hdc);				// free DC
		ReleaseDC(hWnd, memDC);
		ReleaseDC(hWnd, memDC2);
		ReleaseDC(hWnd, memDCallocate);
		ReleaseDC(hWnd, memDCclock);
		ReleaseDC(hWnd, memDChand);
		KillTimer(hWnd, ID_TIMER);
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
	}
	InvalidateRect(hWnd,&lprect,false);
	UpdateWindow(hWnd);

	return 0;
}
int	TextDrawAction(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UINT charCode = (UINT)wParam;
	TCHAR symbol = (TCHAR)charCode;

	int i = 0;
	while (true)
	{
		if (textSymbols[i] == '\0')
		{
			textSymbols[i] = symbol;
			textSymbols[i + 1] = '\0';
			break;
		}
		i++;
	}
	LPSTR str = (LPSTR)textSymbols;
	logbrush.lbStyle = BS_HOLLOW;
	Brush = CreateBrushIndirect(&logbrush);
	SelectObject( memDC2, Brush );
	RECT textRect;

	textRect.left = ptsBegin.x;
	textRect.top = ptsBegin.y;
	textRect.right = ptsBegin.x + 500;
	textRect.bottom = ptsBegin.y + 30;

	FillRect(memDC2, &textRect, Brush);
	logbrush.lbStyle = BS_HOLLOW;
	Brush = CreateBrushIndirect(&logbrush);
	SelectObject( memDC2, Brush );
	DrawText(memDC2,(LPCWSTR)str, -1, &textRect, DT_LEFT);
	BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCAND);
	BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
	InvalidateRect(hWnd, &lprect, false);
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
	case IDM_PEN:		
	case IDM_POLYLINE:		
	case IDM_LINE:		
	case IDM_ELLIPSE:		
	case IDM_RECTANGLE:		
	case IDM_P3:		
	case IDM_P4:		
	case IDM_P5:	
	case IDM_P6:
		fPolyline = FALSE;
		fPrint = FALSE;
		fPan = FALSE;
		fAllocate = FALSE;
		Pen = CreatePen(NULL, width, pcolor);
		SelectObject( memDC2, Pen );
		Brush = CreateBrushIndirect(&oldlogbrush);
		SelectObject( memDC2, Brush );
		BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		break;
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);	
		break;
	case IDM_SAVE:
		GetSaveFile(hWnd);
		break;
	case IDM_OPEN:
		GetOpenFile(hWnd);
		break;
	case IDM_ALLOCATE:
		fAllocate = TRUE;
		Pen = CreatePen(PS_DASH, 1, RGB(128, 128, 128));
	    SelectObject( memDC2, Pen );
		logbrush.lbStyle = BS_HOLLOW;
		Brush = CreateBrushIndirect(&logbrush);
		SelectObject( memDC2, Brush );
		break;
	case IDM_PRINT:
		fPrint = TRUE;
		Pen = CreatePen(PS_DASH, 1, RGB(128, 128, 128));
	    SelectObject( memDC2, Pen );
		logbrush.lbStyle = BS_HOLLOW;
		Brush = CreateBrushIndirect(&logbrush);
		SelectObject( memDC2, Brush );
		break;
	case IDM_CLEAR:
		logbrush.lbColor = RGB(255, 255, 255);
		logbrush.lbStyle = BS_SOLID;
		Brush = CreateBrushIndirect(&logbrush);
		SelectObject( memDC2, Brush );
		FillRect(memDC2, &lprect, Brush);
		BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		InvalidateRect(hWnd, &lprect, false);
		break;
	case IDM_COLORPEN:
		PenColor(hWnd);
		break;
	case IDM_COLORBRUSH:
		BrushColor(hWnd);
		break;
	case IDM_W1:
		PenWidth(1);
		break;
	case IDM_W2:
		PenWidth(2);
		break;
	case IDM_W3:
		PenWidth(3);
		break;
	case IDM_W4:
		PenWidth(4);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
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
int ClockHandPosition(HWND hWnd)
{
	CTime t = CTime::GetCurrentTime();
 
    double m_Second = t.GetSecond() /** 6*/;
    double m_Minute = t.GetMinute() /** 6*/;
    double m_Hour = t.GetHour()/* * 30 + m_Minute/12*/;
	int cX = 150;
	int cY = 150;
	double tmpX;
	double tmpY;
	
	LOGBRUSH l;
	l.lbColor = RGB(255,255,255);
	l.lbStyle = BS_SOLID;
	l.lbHatch = NULL;
	HBRUSH B = CreateBrushIndirect(&l);
	SelectObject( memDChand, B );
	FillRect(memDChand,&lprect, B);

	HPEN P = CreatePen(NULL, 4, RGB(0,0,0));
	SelectObject( memDChand, P );
	MoveToEx(memDChand, cX, cY, (LPPOINT) NULL);
	tmpX = cX + sin((double)(2 * PI * (12 * 60 + m_Minute) / m_Hour / 60))*70;
	tmpY = cY - cos((double)(2 * PI * (12 * 60 + m_Minute) / m_Hour / 60))*70 ;
	LineTo(memDChand, tmpX, tmpY);

	tmpX = cX + sin((double)(2 * PI * m_Minute / 60))*100;
	tmpY = cY - cos((double)(2 * PI * m_Minute / 60))*100;
	MoveToEx(memDChand, cX, cY, (LPPOINT) NULL);
	LineTo(memDChand, tmpX, tmpY);

	P = CreatePen(NULL, 1, RGB(0,0,0));
	SelectObject( memDChand, P );
	MoveToEx(memDChand, cX, cY, (LPPOINT) NULL);
	tmpX = cX + sin((double)(2.0 * PI * m_Second / 60.0))*100;
	tmpY = cY - cos((double)(2.0 * PI * m_Second / 60.0))*100;
	LineTo(memDChand, tmpX, tmpY);

	InvalidateRect(hWnd,&lprect,false);
	UpdateWindow(hWnd);

	DeleteObject(P);
	DeleteObject(B);
	return 0;
}

