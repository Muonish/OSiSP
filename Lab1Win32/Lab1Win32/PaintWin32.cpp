// PaintWin32.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

// Global variables
#define WM_MOUSEMOVE 0x0200
#define W_OPEN 1001
#define W_SAVE 1002
#define W_EXIT 1003
#define W_PEN 1004
#define W_BRUSH 1005
#define W_LINE 1006
#define W_CURVE 1007
#define W_ELLIPSE 1008
#define W_RECTANGLE 1009
#define W_P3 1010
#define W_P4 1011
#define W_P5 1012
#define W_P6 1013
#define W_COLORPEN 1014
#define W_COLORBRUSH 1015
#define W_CLEAR 1016
#define W_PRINT 1017

static TCHAR szWindowClass[] = _T("win32app");			// The main window class name.
static TCHAR szTitle[] = _T("Win32 Paint Application");
static enum figures {NONE, PEN, LINE, CURVE, ELLIPSE, RECTANGLE,TEXT, P3, P4, P5, P6} currentFigure = NONE;
BOOL fTracking = FALSE;
POINTS ptsBegin;
HDC hdc;
HDC memDC;
HDC memDC2;
HBITMAP memBM;
HBITMAP memBM2;
RECT lprect;
HBRUSH Brush;
HGDIOBJ hOldBush;
HPEN Pen;
HGDIOBJ hOldPen;

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int MouseMoveAction(HWND, LPARAM, POINTS *, figures);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Win32 Paint Application"),
            NULL);

        return 1;
    }

    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindow(
        szWindowClass,					//the name of the application
        szTitle,						//the text that appears in the title bar
        WS_OVERLAPPEDWINDOW,			//the type of window to create
        CW_USEDEFAULT, CW_USEDEFAULT,	//initial position (x, y)
        1000, 500,						//initial size (width, length)
        NULL,							//the parent of this window
        NULL,							//this application does not have a menu bar
        hInstance,						//the first parameter from WinMain
        NULL							//not used in this application
    );
	HMENU main_menu = CreateMenu();
	HMENU menu_file = CreatePopupMenu();
	HMENU menu_draw = CreatePopupMenu();
	HMENU menu_figure = CreatePopupMenu();
	HMENU menu_polygon = CreatePopupMenu();
	HMENU menu_color = CreatePopupMenu();
	HMENU menu_canvas = CreatePopupMenu();
	AppendMenu(main_menu, MF_STRING | MF_POPUP, (UINT)menu_file, L"&File");
	AppendMenu(menu_file, MF_STRING, W_OPEN, L"&Open");
	AppendMenu(menu_file, MF_STRING, W_SAVE, L"&Save");
	AppendMenu(menu_file, MF_STRING, W_PRINT, L"&Print");
	AppendMenu(menu_file, MF_STRING, W_EXIT, L"&Exit");
	AppendMenu(main_menu, MF_STRING | MF_POPUP, (UINT)menu_draw, L"&Draw");
	AppendMenu(menu_draw, MF_STRING, W_PEN, L"&Pen");
	AppendMenu(menu_draw, MF_STRING | MF_POPUP, (UINT)menu_figure, L"&Figure");
	AppendMenu(menu_figure, MF_STRING, W_LINE, L"&Line");
	AppendMenu(menu_figure, MF_STRING, W_CURVE, L"&Curve");
	AppendMenu(menu_figure, MF_STRING, W_ELLIPSE, L"&Ellipse");
	AppendMenu(menu_figure, MF_STRING, W_RECTANGLE, L"&Rectangle");  
	AppendMenu(menu_figure, MF_STRING | MF_POPUP, (UINT)menu_polygon, L"&Polygon");  
	AppendMenu(menu_polygon, MF_STRING, W_P3, L"&3");  
	AppendMenu(menu_polygon, MF_STRING, W_P4, L"&4"); 
	AppendMenu(menu_polygon, MF_STRING, W_P5, L"&5"); 
	AppendMenu(menu_polygon, MF_STRING, W_P6, L"&6"); 
	AppendMenu(main_menu, MF_STRING | MF_POPUP, (UINT)menu_color, L"&Color");
	AppendMenu(menu_color, MF_STRING, W_COLORPEN, L"&Pen...");
	AppendMenu(menu_color, MF_STRING, W_COLORBRUSH, L"&Brush...");
	AppendMenu(main_menu, MF_STRING | MF_POPUP, (UINT)menu_canvas, L"&Canvas");
	AppendMenu(menu_canvas, MF_STRING, W_CLEAR, L"&Clear");
 
	SetMenu(hWnd, main_menu);

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Win32 Guided Tour"),
            NULL);

        return 1;
    }

    ShowWindow(hWnd,					// hWnd: the value returned from CreateWindow
        nCmdShow);						// nCmdShow: the fourth parameter from WinMain
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Brush = CreateSolidBrush(RGB(255,255,255));
	
	PAINTSTRUCT ps;

    switch (message)
    {
	case WM_CREATE:
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
<<<<<<< HEAD
		ps.hdc = hdc;
		ps.rcPaint = lprect;
        break;
	case WM_PAINT :
=======
        BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);   // copy from memDC to hdc
        break;
	/*case WM_PAINT:															//IT DOESN'T WORK!!!!!
>>>>>>> parent of c32afb9... lap1_problem_with_runtime
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
		EndPaint(hWnd,&ps);
		break;*/
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);						// capture the mouse
		fTracking = TRUE;
		ptsBegin = MAKEPOINTS(lParam);			// get the begin coords in POINTS format
		break;
	case WM_COMMAND:

		HMENU hMenu;
		
		switch (wParam) 
		{
		case W_OPEN:
			currentFigure = NONE;
			break;
		case W_SAVE:
			currentFigure = NONE;
			break;
		case W_PRINT:
			currentFigure = NONE;
			break;
		case W_EXIT:
			currentFigure = NONE;
			break;
		case W_PEN:
			currentFigure = PEN;
			break;
		case W_LINE:
			currentFigure = LINE;
			break;
		case W_ELLIPSE:
			currentFigure = ELLIPSE;
			break;
		case W_RECTANGLE:
			currentFigure = RECTANGLE;
			break;
		case W_CURVE:
			currentFigure = CURVE;
			break;
		case W_P3:
			currentFigure = P3;
			break;
		case W_P4:
			currentFigure = P4;
			break;
		case W_P5:
			currentFigure = P5;
			break;
		case W_P6:
			currentFigure = P6;
			break;
		case W_COLORPEN:
			
			break;
		case W_COLORBRUSH:
			
			break;
		case W_CLEAR:
			FillRect(memDC,&lprect, Brush);
			FillRect(memDC2,&lprect, Brush);
			InvalidateRect(hWnd,&lprect,false);
			break;
		default: break;
		}
		break;

	case WM_MOUSEMOVE:
		if (fTracking)
		{
			MouseMoveAction(hWnd, lParam, &ptsBegin, currentFigure);
		}
		break;
	case WM_LBUTTONUP:
		BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
		BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
        fTracking = FALSE;
<<<<<<< HEAD
        ClipCursor(NULL);					// free cursor
        ReleaseCapture();
		InvalidateRect(hWnd,&lprect,false);
=======
        ClipCursor(NULL);						// free cursor
        ReleaseCapture();						
>>>>>>> parent of c32afb9... lap1_problem_with_runtime
        return 0;
    case WM_DESTROY:
		ReleaseDC(hWnd, hdc);				// free DC
		ReleaseDC(hWnd, memDC);				// free memDC
		ReleaseDC(hWnd, memDC2);			// free memDC2
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}
int MouseMoveAction(HWND hWnd, LPARAM lParam, POINTS *ptsBegin, figures currentFigure)
{
    POINTS ptsEnd;

	ptsEnd = MAKEPOINTS(lParam);			// get the end coords in POINTS format
	MoveToEx(memDC2, ptsBegin->x, ptsBegin->y, (LPPOINT) NULL);
	Brush = ( HBRUSH ) GetStockObject( HOLLOW_BRUSH );
	hOldBush = SelectObject( memDC2, Brush );
	
		switch (currentFigure)
		{
		case LINE:
			BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
			MoveToEx(memDC2, ptsBegin->x, ptsBegin->y, (LPPOINT) NULL);
			LineTo(memDC2, ptsEnd.x, ptsEnd.y);
			BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
			break;
		case ELLIPSE:
			BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
			Ellipse(memDC2, ptsBegin->x, ptsBegin->y, ptsEnd.x, ptsEnd.y);
			BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
			break;
		case RECTANGLE:
			BitBlt(memDC2, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
			Rectangle(memDC2, ptsBegin->x, ptsBegin->y, ptsEnd.x, ptsEnd.y);
			BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
			break;
		case CURVE:
			LineTo(memDC2, ptsEnd.x, ptsEnd.y);
			BitBlt(memDC, 0, 0, lprect.right, lprect.bottom, memDC2, 0, 0, SRCCOPY);
			BitBlt(hdc, 0, 0, lprect.right, lprect.bottom, memDC, 0, 0, SRCCOPY);
			ptsBegin->x = ptsEnd.x;
			ptsBegin->y = ptsEnd.y;
			break;
		}

	return 0;
}