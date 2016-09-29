#include <Windows.h>
#include <stdio.h>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib, "kernel32.lib")
BOOL CALLBACK EnumFamCallBack(LPLOGFONT, LPNEWTEXTMETRIC, DWORD, LPVOID);
DWORD ListFont();
int main()
{
	ListFont(NULL);
}
//list all font type
DWORD ListFont(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	int aFontCount[] = { 0, 0, 0 };
	EnumFontFamilies(hdc, 
		(LPCSTR)NULL,//all type
		(FONTENUMPROC)EnumFamCallBack,//callback function
		(LPARAM)aFontCount);
	printf("Number of raster fonts:%d\n", aFontCount[0]);
	printf("Number of vector fonts:%d\n", aFontCount[1]);
	printf("Number of TrueType fonts:%d\n", aFontCount[2]);
	return 0;
}
//字体列举回调函数，每列举一次字体会调用一次
BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID aFontCount)
{
	PINT aiFontCount = (PINT)aFontCount;
	if (FontType&RASTER_FONTTYPE)
	{
		printf(" RASTER TYPE\t");
		aiFontCount[0]++;
	}
	else if (FontType&TRUETYPE_FONTTYPE)
	{
		printf(" TRUETYPE\t");
		aiFontCount[2]++;
	}
	else
	{
		printf(" Vector type\t");
		aiFontCount[1]++;
	}
	printf("%s\tItalic = %d\n", lplf->lfFaceName, lplf->lfItalic);
	if (aiFontCount[0] || aiFontCount[1] || aiFontCount[2])
		return TRUE;
	else
		return FALSE;
}