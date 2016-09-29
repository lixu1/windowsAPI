#include <Windows.h>
#include "resource.h"
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

HDC hdcCompat;//handle to DC for bitmap
POINT pt;//鼠标位置
RECT rcTarget;//矩形外延
RECT rcClient;//客户区RECT
//绘制的状态
BOOL fSizeEllipse;
BOOL fDrawEllipse;
BOOL fDrawRectangle;
BOOL fSizeRectangle;
BOOL fSizeRoundRect;
BOOL fDrawRoundRect;
//圆角椭圆的宽和高
int nEllipseWidth;
int nEllipseHeight;
BOOL CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	POINT ptClientUL;
	POINT ptClientLR;
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case ID_SHAPE_ELLIPSE:
			fSizeEllipse = TRUE;
			break;
		case ID_SHAPE_RECTANGLE:
			fSizeRectangle = TRUE;
			break;
		case ID_SHAPE_ROUNDRECT:
			fSizeRoundRect = TRUE;
			break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		break;
	case WM_CREATE:
		//初始化圆角的长和宽
		nEllipseHeight = 20;
		nEllipseWidth = 20;
		return 0;
	case WM_PAINT://绘制
		BeginPaint(hwnd, &ps);
		//选择画刷
		SelectObject(ps.hdc, GetStockObject(GRAY_BRUSH));
		if (fDrawEllipse)//如果有椭圆
		{
			Ellipse(ps.hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			fDrawEllipse = FALSE;
			rcTarget.left = rcTarget.right = 0;
			rcTarget.top = rcTarget.bottom = 0;
		}
		if (fDrawRectangle)//如果有矩形
		{
			Rectangle(ps.hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			fDrawRectangle = FALSE;
			rcTarget.left = rcTarget.right = 0;
			rcTarget.top = rcTarget.bottom = 0;
		}
		if (fDrawRoundRect)//如果有圆角矩形
		{
			RoundRect(ps.hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom, nEllipseWidth, nEllipseHeight);
			fDrawRoundRect = FALSE;
			rcTarget.left = rcTarget.right = 0;
			rcTarget.top = rcTarget.bottom = 0;
		}
		EndPaint(hwnd, &ps);
		break;
	case WM_SIZE://大小改变
		GetClientRect(hwnd, &rcClient);
		ptClientUL.x = rcClient.left;
		ptClientUL.y = rcClient.top;
		ptClientLR.x = rcClient.right;
		ptClientLR.y = rcClient.bottom;
		ClientToScreen(hwnd, &ptClientUL);
		ClientToScreen(hwnd, &ptClientLR);
		//改变RECT的大小和位置
		SetRect(&rcClient, ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y);
		return 0;
	case WM_LBUTTONDOWN:
		//使用鼠标定义一个矩形范围
		ClipCursor(&rcClient);
		//鼠标的位置
		pt.x = (LONG)LOWORD(lParam);
		pt.y = (LONG)HIWORD(lParam);
		if (fDrawEllipse)
			fSizeEllipse = TRUE;
		return 0;
	case WM_MOUSEMOVE:
		if ((wParam&&MK_LBUTTON) && (fSizeEllipse || fSizeRectangle || fSizeRoundRect))
		{
			hdc = GetDC(hwnd);
			//目标RECT是否空
			if (!IsRectEmpty(&rcTarget))
			{
				Rectangle(hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			}
			//根据鼠标移动，得到需要绘制的范围
			if ((pt.x<(LONG)LOWORD(lParam)) && (pt.y>(LONG)HIWORD(lParam)))
			{
				SetRect(&rcTarget, pt.x, HIWORD(lParam), LOWORD(lParam), pt.y);
			}
			else if ((pt.x > (LONG)LOWORD(lParam)) && (pt.y > (LONG)HIWORD(lParam)))
			{
				SetRect(&rcTarget, LOWORD(lParam), HIWORD(lParam), pt.x, pt.y);
			}
			else if ((pt.x > (LONG)LOWORD(lParam)) && (pt.y < (LONG)HIWORD(lParam)))
			{
				SetRect(&rcTarget, LOWORD(lParam), pt.y, pt.x, HIWORD(lParam));
			}
			else
			{
				SetRect(&rcTarget, pt.x, pt.y, LOWORD(lParam), HIWORD(lParam));
			}
			Rectangle(hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			DeleteDC(hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		if (fSizeEllipse)
		{
			fSizeEllipse = FALSE;
			fDrawEllipse = TRUE;
		}
		if (fSizeRectangle)
		{
			fSizeRectangle = FALSE;
			fDrawRectangle = TRUE;
		}
		if (fSizeRoundRect)
		{
			fSizeRoundRect = FALSE;
			fDrawRoundRect = TRUE;
		}
		//是否有绘制的图形：
		if (fDrawEllipse || fDrawRectangle || fDrawRoundRect)
		{
			InvalidateRect(hwnd, &rcTarget, TRUE);
			UpdateWindow(hwnd);
		}
		ClipCursor((LPRECT)NULL);
		return 0;
	case WM_DESTROY:
		DeleteDC(hdcCompat);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return (LRESULT)NULL;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_SHAPE);
	wc.lpszClassName = "shape";
	if (!RegisterClass(&wc))
		return FALSE;
	hWnd = CreateWindow("shape", "shape", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)(msg.wParam);
	UNREFERENCED_PARAMETER(lpCmdLine);
}