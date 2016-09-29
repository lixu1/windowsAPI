#include "cliptext.h"
#include "resource.h"
#include <string.h>
#include <Windows.h>
//#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "gdi32.lib")
//#pragma comment(lib, "winmm.lib")
//#pragma comment(linker, "/subsystem:windows")
#ifdef UNICODE // UNICODE
#define MY_CLIP_FORMAT CF_UNICODETEXT
#else
#define MY_CLIP_FORMAT CF_TEXT
#endif
HINSTANCE hInst;
HACCEL hAccTable;
HWND hwnd;
LPTSTR lpszText = NULL;
LPTSTR lpData = NULL;
HANDLE hHeap;
LPTSTR szInitialClientAreaText =
TEXT("fdsf")
TEXT("fdsvf")
TEXT("ewwf");
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, INT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
VOID OutOfMemory(VOID);
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL fGotMessage;
	if (!InitApplication(hinstance))
	{
		return FALSE;
	}
	if (!InitInstance(hinstance, nCmdShow))
	{
		return FALSE;
	}
	while ((fGotMessage = GetMessage(&msg, (HWND)hwnd, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}
BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("CliptextWClass");
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	return (RegisterClass(&wc));
}

BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
	hInst = hInstance;
	hHeap = GetProcessHeap();
	hAccTable = LoadAccelerators(hInst, TEXT("ClipTextAcc"));
	lpszText = (LPTSTR)HeapAlloc(hHeap, 0, lstrlen(szInitialClientAreaText) + 1);
	lstrcpy(lpszText, szInitialClientAreaText);
	hwnd = CreateWindow(TEXT("CliptextWClass"), TEXT("Cliptext sample Application"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL,
		(HMENU)LoadMenu(hInstance, MAKEINTRESOURCE(ID_CLIP_MENU)),hInstance,NULL);
	if (!hwnd)
	{
		return FALSE;
	}
	ShowWindow(hwnd, nCmdShow);
	return UpdateWindow(hwnd);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hDC = NULL;
	PAINTSTRUCT ps = { 0 };
	RECT rectClient = { 0 };
	LPTSTR lpClipData = NULL;
	switch (message)
	{
	case WM_INITMENU:
		if (wParam == (WPARAM)GetMenu(hWnd))
		{
			if (OpenClipboard(hWnd))
			{
				if (IsClipboardFormatAvailable(MY_CLIP_FORMAT) || IsClipboardFormatAvailable(CF_OEMTEXT))
				{
					EnableMenuItem((HMENU)wParam, IDM_PASTE, MF_ENABLED);
				}
				else
				{
					EnableMenuItem((HMENU)wParam, IDM_PASTE, MF_GRAYED);
				}
				CloseClipboard();
				return TRUE;
			}
			else
				return FALSE;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT:
			break;
		case IDM_NEW:
		case IDM_SAVE:
		case IDM_SAVEAS:
		case IDM_PRINT:
		case IDM_UNDO:
		case IDM_CLEAR:
			MessageBox(GetFocus(), TEXT("Command not implemented"), TEXT("ClipText sample application"), MB_ICONASTERISK | MB_OK);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_CUT:
		case IDM_COPY:
			if (lpszText != NULL)
			{
				if (lpData != NULL)
				{
					HeapFree(hHeap, 0, lpData);
				}
				lpData =(LPTSTR) HeapAlloc(hHeap, 0, HeapSize(hHeap, 0, lpszText));
				lstrcpy(lpData, lpszText);
				if (OpenClipboard(hWnd))
				{
					EmptyClipboard();
					SetClipboardData(MY_CLIP_FORMAT, lpData);
				}
				CloseClipboard();
				if (LOWORD(wParam) == IDM_CUT)
				{
					HeapFree(hHeap, 0, lpszText);
					lpszText = 0;
					EnableMenuItem(GetMenu(hWnd), IDM_CUT, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDM_COPY, MF_GRAYED);
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}
			}
			return TRUE;
		case IDM_PASTE:
			if (OpenClipboard(hWnd))
			{
				if (!(lpClipData = GetClipboardData(MY_CLIP_FORMAT)))
				{
					CloseClipboard();
					break;
				}
				if (lpszText != NULL)
				{
					HeapFree(hHeap, 0, lpszText);
					lpszText = NULL;
				}
				lpszText = HeapAlloc(hHeap, 0, lstrlen(lpClipData) + 1);
				lstrcpy(lpszText, lpClipData);
				CloseClipboard();
				EnableMenuItem(GetMenu(hWnd), IDM_CUT, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_COPY, MF_ENABLED);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				return (TRUE);
			}
			else
				return FALSE;
		}
		break;
	case WM_SIZE:
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		//显示文本到界面上
		hDC = BeginPaint(hWnd, &ps);
		if (lpszText != NULL)
		{
			GetClientRect(hWnd, &rectClient);
			DrawText(hDC, lpszText, -1, &rectClient, DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		//释放内存，退出
		if (lpszText != NULL)
		{
			HeapFree(hHeap, 0, lpszText);
		}
		if (lpData != NULL)
		{
			HeapFree(hHeap, 0, lpData);
		}
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}