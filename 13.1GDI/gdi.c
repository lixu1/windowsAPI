#include <Windows.h>
#include <stdio.h>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(linker, "/subsystem:windows")
void GdiOut(HDC hdc);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HDC hdc = GetDC(NULL);
	GdiOut(hdc);
	ReleaseDC(NULL, hdc);
}
VOID GdiOut(HDC hdc)
{
	HPEN hpen, hpenOld;
	HBRUSH hbrush, hbrushOld;
	BYTE bRed = 0;
	BYTE bGreen = 0;
	BYTE bBlue = 0;
	COLORREF cPen = RGB(bRed, bGreen, bBlue);
	COLORREF cBrush = RGB(233, 100/*GetValue(cPen)*/, 255);
	hpen = CreatePen(PS_SOLID, 10, cPen);
	hbrush = CreateSolidBrush(cBrush);
	hpenOld = SelectObject(hdc, hpen);
	hbrushOld = SelectObject(hdc, hbrush);
	LineTo(hdc, 500, 500);
	SelectObject(hdc, hpenOld);
	Rectangle(hdc, 200, 200, 500, 500);
	DeleteObject(hpen);
	SelectObject(hdc, hpenOld);
	DeleteObject(hbrush);
}