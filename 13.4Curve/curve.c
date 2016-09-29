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
	//ע�ᴰ����
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
	//��������
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
		//��ȡDC
		HDC hDC = GetDC(hWnd);
		//��ȡ���ʶ���
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
		//���ͻ�������Ϊ��ɫ
		GetClientRect(hWnd, &rect);
		PatBlt(hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
		//�ȴ��ں����ִ�������
		if (dwBlue&&dwRed)
		{
			//��ʾ����
			if (dwBlue&&bShowGuides)
			{
				SelectObject(hDC, hpenBlue);
				ShowGuide(hDC, lpBlue, dwBlue);
				SelectObject(hDC, GetStockObject(BLACK_PEN));
			}
			//��ʾ����
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
		//������������
		if (bOn)
		{
			//���ͻ�ȥ���Ϊ��ɫ
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
		//������
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
//������꣬�������켣
BOOL GetGuideLine(HWND hWnd, LPPOINT *lpPoint, LPDWORD lpdwNumPts)
{
	MSG msg;
	HDC hDC = GetDC(hWnd);
	BOOL bFirstTime = TRUE;
	DWORD dwPos = 0;
	RECT rect;
	SetCapture(hWnd);//������겶����
	GetClientRect(hWnd, &rect);
	//Ϊ���������ռ�
	*lpPoint = (LPPOINT)MyAlloc(MAXGUIDESEGMENTS*sizeof(POINT));
	for (;;)
	{
		//�������������Ϣ
		WaitMessage();
		if (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
		{
			//�ж��Ƿ��ڿͻ�����
			if ((LOWORD(msg.lParam) < rect.right) && (HIWORD(msg.lParam) < rect.bottom))
			{
				//�Ƿ��һ���յ���Ϣ
				if (bFirstTime)
				{
					bFirstTime = FALSE;
					//��һ�Σ������ʵ�����ƶ�����굥����λ��
					MoveToEx(hDC, LOWORD(msg.lParam), HIWORD(msg.lParam), NULL);
				}
				//�Ƿ�ﵽ������
				if (dwPos < MAXGUIDESEGMENTS)
				{
					//����ƶ�����������Ϣ��ÿ�ܵ�һ����Ϣ������һ����
					(*lpPoint)[dwPos].x = LOWORD(msg.lParam);
					(*lpPoint)[dwPos].y = HIWORD(msg.lParam);
					//���Ƶ�������ڵĵ�
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
//���ݱ���ĵ�����飬�ػ�����
BOOL ShowGuide(HDC hDC, LPPOINT lpPoints, DWORD dwNumPts)
{
	Polyline(hDC, lpPoints, dwNumPts);
	return TRUE;
}
//��ʾ������Ϣ
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