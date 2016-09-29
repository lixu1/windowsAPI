#include <Windows.h>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib,"comdlg32.lib")// error LNK2019: unresolved external symbol __imp__ChooseFontA@4 referenced in function _ChooseNewFont
HFONT ChooseNewFont()
{
	CHOOSEFONT cf;
	LOGFONT lf;
	HFONT hfont;
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = (HWND)NULL;
	cf.hDC = (HDC)NULL;
	cf.lpLogFont = &lf;
	cf.iPointSize = 0;
	cf.Flags = CF_SCREENFONTS;
	cf.rgbColors = RGB(0, 0, 0);
	cf.lCustData = 0L;
	cf.lpfnHook = (LPCFHOOKPROC)NULL;
	cf.lpTemplateName = (LPSTR)NULL;
	cf.hInstance = (HINSTANCE)NULL;
	cf.lpszStyle = (LPSTR)NULL;
	cf.nFontType = SCREEN_FONTTYPE;
	cf.nSizeMax = 0;
	cf.nSizeMin = 0;
	ChooseFont(&cf);
	hfont = CreateFontIndirect(cf.lpLogFont);
	return hfont;
}
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HDC hdc = GetDC(NULL);
	int XIncrement = 10;
	int YStart = 50;
	TEXTMETRIC tm;
	HFONT hfntDefault, hfntChoose;
	SIZE sz;
	UINT uAlignPrev;
	LPSTR lpszString1 = "string 1";
	LPSTR lpszString2 = "string 2";
	LPSTR lpszString3 = "string 3";
	DWORD dwStrLen1 = lstrlen(lpszString1);
	DWORD dwStrLen2 = lstrlen(lpszString2);
	DWORD dwStrLen3 = lstrlen(lpszString3);

	//choose font
	hfntChoose = ChooseNewFont();
	//set color
	SetBkColor(hdc, RGB(255, 255, 255));
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkMode(hdc, TRANSPARENT);
	//output string1
	TextOut(hdc, XIncrement, YStart, lpszString1, dwStrLen1);

	//set string2 location
	GetTextExtentPoint32(hdc, lpszString1, dwStrLen1, &sz);
	XIncrement += sz.cx;
	GetTextMetrics(hdc, &tm);
	XIncrement -= tm.tmOverhang;
	//choose font
	hfntDefault = SelectObject(hdc, hfntChoose);
	TextOut(hdc, XIncrement, YStart, lpszString2, dwStrLen2);

	//set string3 location
	GetTextExtentPoint32(hdc, lpszString1, dwStrLen1, &sz);
	XIncrement = 10;
	YStart += sz.cy;
	GetTextMetrics(hdc, &tm);
	XIncrement -= tm.tmOverhang;
	//set font
	SelectObject(hdc, hfntDefault);
	uAlignPrev = SetTextAlign(hdc, TA_UPDATECP);
	MoveToEx(hdc, XIncrement, YStart, (LPPOINT)NULL);
	TextOut(hdc, 0, 0, lpszString3, dwStrLen3);
	SetTextAlign(hdc, uAlignPrev);

	//clear
	DeleteObject(hfntChoose);
	SetBkMode(hdc, OPAQUE);
	DeleteDC(hdc);
	return 0;
}