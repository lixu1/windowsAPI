#include <Windows.h>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#define MAXGUIDESEGMENTS 1000
#define MyAlloc(dwSize) HeapAlloc(GetProcessHeap(),0,dwSize);
#define MyFree(lpMem) HeapFree(GetProcessHeap(),0,lpMem);
BOOL GetGuideLine(HWND, LPPOINT *, LPDWORD);
BOOL ShowGuide(HDC, LPPOINT, DWORD);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD GetLastErrorBox(HWND hwnd, LPSTR lpTitle);
HINSTANCE hInst;
LPSTR szAppName = "Curves";
LPSTR szTitle = "Curves Application";
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreveInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;
	hInst = hInstance;
	//注册窗口类
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
	if (!RegisterClass(&wc))
	{
		GetLastErrorBox(NULL, "Error in registerclass");
		return FALSE;
	}
	//创建窗口
	hWnd = CreateWindow(szAppName, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		GetLastErrorBox(hWnd, "Error in CreateWindow");
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)(msg.wParam);
	UNREFERENCED_PARAMETER(lpCmdLine);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam)
{
	static BOOL bOn = TRUE;
	static LPPOINT lpBlue = NULL;
	static LPPOINT lpRed = NULL;
	static LPPOINT dwBlue = 0;
	static DWORD dwRed = 0;
	static BOOL bOutlineOnly = FALSE;
	static BOOL bShowGuides = TRUE;
	static HPEN hpenBlue, hpenRed;
	switch (message)
	{
	case WM_CREATE:
	{
		//获取DC
		HDC hDC = GetDC(hWnd);
		//获取画笔对象
		hpenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		hpenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	}
	GetLastErrorBox(hWnd, "Error in WM_CREATE");
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		RECT rect;
		//将客户区绘制为白色
		GetClientRect(hWnd, &rect);
		PatBlt(hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
		//既存在红线又存在蓝线
		if (dwBlue&&dwRed)
		{
			//显示蓝线
			if (dwBlue&&bShowGuides)
			{
				SelectObject(hDC, hpenBlue);
				ShowGuide(hDC, lpBlue, dwBlue);
				SelectObject(hDC, GetStockObject(BLACK_PEN));
			}
			//显示蓝线
			if (dwRed&&bShowGuides)
			{
				SelectObject(hDC, hpenRed);
				ShowGuide(hDC, lpRed, dwRed);
				SelectObject(hDC, GetStockObject(BLACK_PEN));
			}
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		HDC hDC = GetDC(hWnd);
		RECT rect;
		//消除并画蓝线
		if (bOn)
		{
			//将客户去填充为白色
			GetClientRect(hWnd, &rect);
			PatBlt(hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
			if (lpBlue)
				MyFree(lpBlue);
			if (lpRed)
				MyFree(lpRed);
			dwRed = 0;
			dwBlue = 0;
			SelectObject(hDC, hpenBlue);
			GetGuideLine(hWnd, &lpBlue, &dwBlue);
		}
		//划红线
		else
		{
			SelectObject(hDC, hpenRed);
			GetGuideLine(hWnd, &lpRed, &dwRed);
		}
		SelectObject(hDC, GetStockObject(BLACK_PEN));
		bOn = !bOn;
	}
	GetLastErrorBox(hWnd, "Error in WM_LBUTTONDOWN");
	break;
	case WM_DESTROY:
		if (lpBlue)MyFree(lpBlue);
		if (lpRed)MyFree(lpRed);
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, message, uParam, lParam));
	}
	return 0;
}
//跟踪鼠标，绘制鼠标轨迹
BOOL GetGuideLine(HWND hWnd, LPPOINT *lpPoint, LPDWORD lpdwNumPts)
{
	MSG msg;
	HDC hDC = GetDC(hWnd);
	BOOL bFirstTime = TRUE;
	DWORD dwPos = 0;
	RECT rect;
	SetCapture(hWnd);//设置鼠标捕获器
	GetClientRect(hWnd, &rect);
	//为点数组分配空间
	*lpPoint = (LPPOINT)MyAlloc(MAXGUIDESEGMENTS*sizeof(POINT));
	for (;;)
	{
		//过滤所有鼠标消息
		WaitMessage();
		if (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
		{
			//判断是否在客户区中
			if ((LOWORD(msg.lParam) < rect.right) && (HIWORD(msg.lParam) < rect.bottom))
			{
				//是否第一次收到消息
				if (bFirstTime)
				{
					bFirstTime = FALSE;
					//第一次，将画笔的起点移动到鼠标单击的位置
					MoveToEx(hDC, LOWORD(msg.lParam), HIWORD(msg.lParam), NULL);
				}
				//是否达到最大点数
				if (dwPos < MAXGUIDESEGMENTS)
				{
					//鼠标移动会产生鼠标消息，每受到一次消息，保存一个点
					(*lpPoint)[dwPos].x = LOWORD(msg.lParam);
					(*lpPoint)[dwPos].y = HIWORD(msg.lParam);
					//绘制到鼠标所在的点
					LineTo(hDC, (*lpPoint)[dwPos].x, (*lpPoint)[dwPos].y);
					dwPos++;
				}
			}
			if (msg.message == WM_LBUTTONUP)
				break;
		}
		else
			continue;
	}
	*lpdwNumPts = dwPos;
	ReleaseDC(hWnd, hDC);
	ReleaseCapture();
	DeleteDC(hDC);
	return TRUE;
}
//根据保存的点的数组，重绘曲线
BOOL ShowGuide(HDC hDC, LPPOINT lpPoints, DWORD dwNumPts)
{
	Polyline(hDC, lpPoints, dwNumPts);
	return TRUE;
}
//显示错误信息
DWORD GetLastErrorBox(HWND hWnd, LPSTR lpTitle)
{
	LPVOID lpv;
	DWORD dwRv;
	if (GetLastError() == 0)return 0;
	dwRv = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPVOID)&lpv, 0, NULL);
	MessageBox(hWnd, lpv, lpTitle, MB_OK);
	if (dwRv)
		LocalFree(lpv);
	SetLastError(0);
	return dwRv;
}