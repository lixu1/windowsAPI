#include <Windows.h>
#include <commctrl.h>
#include "resource.h"
#include <winuser.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Gdi32.lib")
HINSTANCE hinst;
UINT uFormat = (UINT)(-1);
BOOL fAuto = TRUE;
HWND hwnd;
LPSTR szFormatText = "CF_TEXT";
LPSTR szFormatOwnerdisplay = "CF_OWNERDISPLAY";
LPSTR szFormatEnhmetafile = "CF_ENHMETAFILE";
LPSTR szFormatBitMap = "CF_BITMAP";
void WINAPI InitMenu(HWND hwnd, HMENU hmenu);
BOOL WINAPI IsDisplayaleFormat(UINT uFormat);
void WINAPI SetAutoView(HWND hwnd);
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LPSTR GetPredefinedClipboardFormatName(UINT);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
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
	while ((fGotMessage = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}
BOOL InitApplication(HINSTANCE hinstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);//结构大小
	wcx.style = CS_HREDRAW | CS_VREDRAW;//如果大小改变，重绘窗口
	wcx.lpfnWndProc = MainWndProc;//窗口消息处理函数
	wcx.cbClsExtra = 0;//无附加窗口类内存
	wcx.cbWndExtra = 0;//无附加窗口内存
	wcx.hInstance = hinstance;//应用程序实例
	wcx.hIcon = NULL;//图标
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);//鼠标指针
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//背景画刷
	wcx.lpszMenuName = NULL;//MAKEINTRESOURCE(IDR_MENU_MAIN) 菜单资源
	wcx.lpszClassName = "MainWClass";//窗口类名
	wcx.hIconSm = NULL;
	return RegisterClassEx(&wcx);
}
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	hinst = hinstance;
	hwnd = CreateWindow("MainWClass", 
		"ClipBoradViewer",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUPWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, 
		(HWND)NULL, 
		(HMENU)LoadMenu(hinst, MAKEINTRESOURCE(IDR_MENU_COMMAND)),
		hinstance, 
		(LPVOID)NULL);
	if (!hwnd)
		return FALSE;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}
//窗口消息处理函数
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndNextViewer;
	HDC hdc;
	HDC hdcMem;
	PAINTSTRUCT ps;
	LPPAINTSTRUCT lpps;
	RECT rc;
	LPRECT lprc;
	HGLOBAL hglb;
	LPSTR lpstr;
	HBITMAP hbm;
	HENHMETAFILE hemf;
	HWND hwndOwner;
	switch (uMsg)
	{
	case WM_PAINT:
		//显示剪切板中的数据
		hdc = BeginPaint(hwnd, &ps);//uFormat全局变量，在WM_DRAWCLIPBOARD和WM_COMMAND消息处理中，使用SetAutoView设置，根据剪切板中数据的不同格式，使用不同的显示方式
		switch (uFormat)
		{
		case CF_OWNERDISPLAY:
			//Owner-display格式，剪切板的所有者必须显示并刷新Viewer的窗口
			hwndOwner = GetClipboardOwner();//获取剪切板的所有者
			hglb = GlobalAlloc(GMEM_MOVEABLE, sizeof(PAINTSTRUCT));//获取剪切板数据
			lpps = GlobalLock(hglb);
			memcpy(lpps, &ps, sizeof(PAINTSTRUCT));
			GlobalUnlock(hglb);
			//向剪切板所有者发送WM_PAINTCLIPBOARD消息
			SendMessage(hwndOwner, WM_PAINTCLIPBOARD, (WPARAM)hwnd, (LPARAM)hglb);
			GlobalFree(hglb);
			break;
		case CF_BITMAP://位图
			hdcMem = CreateCompatibleDC(hdc);//创建DC
			if (hdcMem != NULL)
			{
				if (OpenClipboard(hwnd))
				{
					hbm = (HBITMAP)GetClipboardData(uFormat);
					SelectObject(hdcMem, hbm);
					GetClientRect(hwnd, &rc);
					BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
					CloseClipboard();
				}
				DeleteDC(hdcMem);
			}
			break;
		case CF_TEXT:
			if (OpenClipborad(hwnd))
			{
				hglb = GetClipboardData(uFormat);
				lpstr = GlobalLock(hglb);
				GetClientRect(hwnd, &rc);
				DrawText(hdc, lpstr, -1, &rc, DT_LEFT);
				GlobalUnlock(hglb);
				CloseClipboard();
			}
			break;
		case CF_ENHMETAFILE:
			if (OpenClipBorad(hwnd))
			{
				hemf = GetClipboardData(uFormat);
				GetClientRect(hwnd, &rc);
				PlayEnhMetaFile(hdc, hemf, &rc);
				CloseClipboard();
			}
			break;
		case 0:
			GetClientRect(hwnd, &rc);
			DrawText(hdc, "The Clipborad is empty", -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			break;
		default:
			GetClientRect(hwnd, &rc);
			DrawText(hdc, "Unable to display format", -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
		EndPaint(hwnd, &ps);
		break;
	case WM_SIZE:
		if (uFormat == CF_OWNERDISPLAY)
		{
			hwndOwner = GetClipboardOwnder();
			hglb = GlobalAlloc(GMEM_MOVEABLE, sizeof(RECT));
			lprc = GlobalLock(hglb);
			GetClientRect(hwnd, lprc);
			GlobalUnlock(hglb);
			SendMessage(hwndOwner, WM_SIZECLIPBOARD, (WPARAM)hwnd, (LPARAM)hglb);
			GlobalFree(hglb);
		}
		break;
	case WM_CREATE:
		hwndNextViewer = SetClipboardViewer(hwnd);//在窗口创建时，在剪切板Viewer链中增加一个Viewer
		break;
	case WM_CHANGECBCHAIN:
		//在链中增加本viewer 后，位于链的第一个节点，当链中有Viewer移除时，链中的第一个Viewer会收到此消息，如果本Viewer的下一个Viewer移除，那么更新
		if ((HWND)wParam == hwndNextViewer)
			hwndNextViewer = (HWND)lParam;
		//否则将消息向下一个Viewer传递
		else if (hwndNextViewer != NULL)
			SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		ChangeClipboardChain(hwnd, hwndNextViewer);
		PostQuitMessage(0);
		break;
	case WM_DRAWCLIPBOARD:
		SetAutoView(hwnd);
		SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		break;
	case WM_INITMENUPOPUP:
		if (!HIWORD(lParam))
			InitMenu(hwnd, (HMENU)wParam);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_NULL:
			break;
		case ID_AUTO://点击Auto菜单项
			SetAutoView(hwnd);//设置显示格式为自动
			break;
		default://用户选择了特定的显示格式
			fAuto = FALSE;
			uFormat = LOWORD(wParam);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return (LRESULT)NULL;
}
//设置剪切板的主要格式，并设置显示方式
void WINAPI SetAutoView(HWND hwnd)
{
	static UINT auPriorityList[] = {
		CF_OWNERDISPLAY, CF_TEXT, CF_ENHMETAFILE, CF_BITMAP
	};
	//uFormat在WM_PAINT消息时引用
	uFormat = GetPriorityClipboardFormat(auPriorityList, 4);
	fAuto = TRUE;
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}
void WINAPI InitMenu(HWND hwnd, HMENU hmenu)
{
	UINT uFormat;
	char szFormatName[80];
	LPCSTR lpFormatName;
	UINT fuFlags;
	UINT idMenuItem;
	if (GetMenuItemID(hmenu, 0) != ID_AUTO)
		return;
	while (GetMenuItemCount(hmenu) > 1)
		DeleteMenu(hmenu, 1, MF_BYPOSITION);
	fuFlags = fAuto ? MF_BYCOMMAND | MF_BYPOSITION:MF_BYCOMMAND|MF_UNCHECKED;
	CheckMenuItem(hmenu, ID_AUTO, fuFlags);
	if (CountClipboardFormats() == 0)
		return;
	if (!OpenClipboard(hwnd))
		return;
	AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);
	uFormat = EnumClipboardFormats(0);
	while (uFormat)
	{
		lpFormatName = GetPredefinedClipboardFormatName(uFormat);
		if (lpFormatName == NULL)
		{
			if (GetClipboardFormatName(uFormat, szFormatName, sizeof(szFormatName)))
				lpFormatName = szFormatName;
			else
				lpFormatName = "(unknown)";
		}
		if (IsDisplayaleFormat(uFormat))
		{
			fuFlags = MF_STRING;
			idMenuItem = uFormat;
		}
		else
		{
			fuFlags = MF_STRING | MF_GRAYED;
			idMenuItem = 0;
		}
		AppendMenu(hmenu, fuFlags, idMenuItem, lpFormatName);
		uFormat = EnumClipboardFormats(uFormat);
	}
	CloseClipboard();
}
//根据格式返回格式名字符串，用于在菜单上显示
LPSTR GetPredefineClipboardFormatName(UINT uFormat)
{
	switch (uFormat)
	{
	case CF_OWNERDISPLAY:
		return szFormatOwnerdisplay;
	case CF_TEXT:
		return szFormatText;
	case CF_ENHMETAFILE:
		return szFormatEnhmetafile;
	case CF_BITMAP:
		return szFormatBitMap;
	default:
		return NULL;
	}
}
//判断是否为可显示的格式
BOOL WINAPI IsDisplayableFormat(UINT uFormat)
{
	switch (uFormat)
	{
	case CF_OWNERDISPLAY:
	case CF_TEXT:
	case CF_ENHMETAFILE:
	case CF_BITMAP:
		return TRUE;
	}
	return FALSE;
}