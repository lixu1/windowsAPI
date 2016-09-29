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
	printf("内存中的内容：\"%s\",内存地址：0x%.8x,内存属性：%u\n", lpMem, lpMem, uFlags);
	GlobalFree(lpMem);
	hMem = GlobalAlloc(GMEM_MOVEABLE, 1000);
	sizeMem = GlobalSize(hMem);
	uFlags = GlobalFlags(hMem);
	printf("内存大小：%d,内存句柄:0x%.8x,内存属性：%u\n", sizeMem, hMem, uFlags);
	lpMem = GlobalLock(hMem);
	return 0;
}