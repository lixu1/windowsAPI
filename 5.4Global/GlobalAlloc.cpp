#include <Windows.h>
#include <stdio.h>
int main(void)
{
	LPVOID lpMem;
	HGLOBAL hMem;
	SIZE_T sizeMem;
	UINT uFlags;
	lpMem = (LPVOID)GlobalAlloc(GPTR, 1000);
	//lstrcpy(lpMem,"this is a string");
	uFlags = GlobalFlags(lpMem);
	printf("�ڴ��е����ݣ�\"%s\",�ڴ��ַ��0x%.8x,�ڴ����ԣ�%u\n", lpMem, lpMem, uFlags);
	GlobalFree(lpMem);
	hMem = GlobalAlloc(GMEM_MOVEABLE, 1000);
	sizeMem = GlobalSize(hMem);
	uFlags = GlobalFlags(hMem);
	printf("�ڴ��С��%d,�ڴ���:0x%.8x,�ڴ����ԣ�%u\n", sizeMem, hMem, uFlags);
	lpMem = GlobalLock(hMem);
	return 0;
}