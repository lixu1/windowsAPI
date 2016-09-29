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
	wcx.cbSize = sizeof(wcx);//�ṹ��С
	wcx.style = CS_HREDRAW | CS_VREDRAW;//�����С�ı䣬�ػ洰��
	wcx.lpfnWndProc = MainWndProc;//������Ϣ������
	wcx.cbClsExtra = 0;//�޸��Ӵ������ڴ�
	wcx.cbWndExtra = 0;//�޸��Ӵ����ڴ�
	wcx.hInstance = hinstance;//Ӧ�ó���ʵ��
	wcx.hIcon = NULL;//ͼ��
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);//���ָ��
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//������ˢ
	wcx.lpszMenuName = NULL;//MAKEINTRESOURCE(IDR_MENU_MAIN) �˵���Դ
	wcx.lpszClassName = "MainWClass";//��������
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
//������Ϣ������
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
		//��ʾ���а��е�����
		hdc = BeginPaint(hwnd, &ps);//uFormatȫ�ֱ�������WM_DRAWCLIPBOARD��WM_COMMAND��Ϣ�����У�ʹ��SetAutoView���ã����ݼ��а������ݵĲ�ͬ��ʽ��ʹ�ò�ͬ����ʾ��ʽ
		switch (uFormat)
		{
		case CF_OWNERDISPLAY:
			//Owner-display��ʽ�����а�������߱�����ʾ��ˢ��Viewer�Ĵ���
			hwndOwner = GetClipboardOwner();//��ȡ���а��������
			hglb = GlobalAlloc(GMEM_MOVEABLE, sizeof(PAINTSTRUCT));//��ȡ���а�����
			lpps = GlobalLock(hglb);
			memcpy(lpps, &ps, sizeof(PAINTSTRUCT));
			GlobalUnlock(hglb);
			//����а������߷���WM_PAINTCLIPBOARD��Ϣ
			SendMessage(hwndOwner, WM_PAINTCLIPBOARD, (WPARAM)hwnd, (LPARAM)hglb);
			GlobalFree(hglb);
			break;
		case CF_BITMAP://λͼ
			hdcMem = CreateCompatibleDC(hdc);//����DC
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
		hwndNextViewer = SetClipboardViewer(hwnd);//�ڴ��ڴ���ʱ���ڼ��а�Viewer��������һ��Viewer
		break;
	case WM_CHANGECBCHAIN:
		//���������ӱ�viewer ��λ�����ĵ�һ���ڵ㣬��������Viewer�Ƴ�ʱ�����еĵ�һ��Viewer���յ�����Ϣ�������Viewer����һ��Viewer�Ƴ�����ô����
		if ((HWND)wParam == hwndNextViewer)
			hwndNextViewer = (HWND)lParam;
		//������Ϣ����һ��Viewer����
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
		case ID_AUTO://���Auto�˵���
			SetAutoView(hwnd);//������ʾ��ʽΪ�Զ�
			break;
		default://�û�ѡ�����ض�����ʾ��ʽ
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
//���ü��а����Ҫ��ʽ����������ʾ��ʽ
void WINAPI SetAutoView(HWND hwnd)
{
	static UINT auPriorityList[] = {
		CF_OWNERDISPLAY, CF_TEXT, CF_ENHMETAFILE, CF_BITMAP
	};
	//uFormat��WM_PAINT��Ϣʱ����
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
//���ݸ�ʽ���ظ�ʽ���ַ����������ڲ˵�����ʾ
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
//�ж��Ƿ�Ϊ����ʾ�ĸ�ʽ
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