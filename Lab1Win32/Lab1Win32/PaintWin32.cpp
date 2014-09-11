// PaintWin32.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

// Global variables
#define WM_MOUSEMOVE 0x0200


static TCHAR szWindowClass[] = _T("win32app");			// The main window class name.
static TCHAR szTitle[] = _T("Win32 Paint Application");

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
    HDC hdc;

    static POINTS ptsBegin;
    static POINTS ptsEnd;
    static BOOL fTracking = FALSE;

    switch (message)
    {
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);						// capture the mouse
		fTracking = TRUE;

		ptsBegin = MAKEPOINTS(lParam);			// get the begin coords in POINTS format
		return 0;

	case WM_MOUSEMOVE:
		if (fTracking)
		{

			hdc = GetDC(hWnd);					// retrieves a handle to a device context (DC) for the client area

			ptsEnd = MAKEPOINTS(lParam);		// get the end coords in POINTS format
			MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT) NULL);
			LineTo(hdc, ptsEnd.x, ptsEnd.y);
			ptsBegin.x = ptsEnd.x;
			ptsBegin.y = ptsEnd.y;

			ReleaseDC(hWnd, hdc);				// free DC
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