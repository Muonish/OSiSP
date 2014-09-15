// PaintWin32.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

// Global variables
#define WM_MOUSEMOVE 0x0200
#define W_PEN 1002
#define W_BRUSH 1003
#define W_LINE 1004
#define W_ELLIPSE 1005
#define W_RECTANGLE 1006


static TCHAR szWindowClass[] = _T("win32app");			// The main window class name.
static TCHAR szTitle[] = _T("Win32 Paint Application");
static enum tools {PEN, BRUSH} currentTool = PEN;
static enum figures {NONE, LINE, ELLIPSE, RECTANGLE} currentFigure = NONE;
BOOL fTracking = FALSE;
POINTS ptsBegin;

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int MouseMoveAction(HWND, LPARAM, POINTS *, figures , tools );

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
	AppendMenu(main_menu, MF_STRING | MF_POPUP, (UINT)menu_file, L"&File");
	AppendMenu(main_menu, MF_STRING | MF_POPUP, (UINT)menu_draw, L"&Draw");
	AppendMenu(menu_file, MF_STRING, 1001, L"...");
	AppendMenu(menu_draw, MF_STRING, W_PEN, L"&Pen");
	AppendMenu(menu_draw, MF_STRING, W_BRUSH, L"&Brush");
	AppendMenu(menu_draw, MF_STRING | MF_POPUP, (UINT)menu_figure, L"&Figure");
	AppendMenu(menu_figure, MF_STRING, W_LINE, L"&Line");
	AppendMenu(menu_figure, MF_STRING, W_ELLIPSE, L"&Ellipse");
	AppendMenu(menu_figure, MF_STRING, W_RECTANGLE, L"&Rectangle");
	//CheckMenuRadioItem(menu_draw,W_PEN,W_BRUSH,W_PEN, MF_CHECKED);  
 
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

    switch (message)
    {
	case WM_CREATE:
        
        break;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);						// capture the mouse
		fTracking = TRUE;
		ptsBegin = MAKEPOINTS(lParam);			// get the begin coords in POINTS format
		break;
	case WM_COMMAND:

		HMENU hMenu;
		switch (wParam) 
		{
		case W_PEN:
			currentTool = PEN;
			break;
		case W_BRUSH:
			currentTool = BRUSH;
			break;
		default: break;
		}

		switch (wParam) 
		{
		case W_LINE:
			currentFigure = LINE;
			break;
		case W_ELLIPSE:
			currentFigure = ELLIPSE;
			break;
		case W_RECTANGLE:
			currentFigure = RECTANGLE;
			break;
		default: break;
		}
		break;

	case WM_MOUSEMOVE:
		if (fTracking)
		{
			MouseMoveAction(hWnd, lParam, &ptsBegin, currentFigure, currentTool);
		}
		break;
    case WM_LBUTTONUP:

        fTracking = FALSE;
        ClipCursor(NULL);						// free cursor
        ReleaseCapture();						
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}
int MouseMoveAction(HWND hWnd, LPARAM lParam, POINTS *ptsBegin, figures currentFigure, tools currentTool)
{
	HDC hdc;
    POINTS ptsEnd;
	RECT lprect;
	hdc = GetDC(hWnd);					// retrieves a handle to a device context (DC) for the client area
	//CreateCompatibleDC(hdc);
	//GetClientRect(hWnd, &lprect);
	//CreateCompatibleBitmap(hdc, lprect.right, lprect.bottom);

	ptsEnd = MAKEPOINTS(lParam);		// get the end coords in POINTS format
	MoveToEx(hdc, ptsBegin->x, ptsBegin->y, (LPPOINT) NULL);
	if (currentTool == PEN)
	{
		switch (currentFigure)
		{
		case LINE:

			break;
		case ELLIPSE:

			break;
		case RECTANGLE:

			break;
		default:
			LineTo(hdc, ptsEnd.x, ptsEnd.y);
			ptsBegin->x = ptsEnd.x;
			ptsBegin->y = ptsEnd.y;
			break;
		}
	}
	if (currentTool == BRUSH)
	{
	}
		
	ReleaseDC(hWnd, hdc);				// free DC
	return 0;
}