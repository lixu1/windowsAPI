#include <Windows.h>
#include <stdio.h>
#define MEM_BLOCK_SIZE 32
BOOL ShowMemContent(LPVOID lpMem, SIZE_T dwSize)
{
	BYTE lpShow[MEM_BLOCK_SIZE];
	INT i = 0;
	if (dwSize > MEM_BLOCK_SIZE)
	{
		printf("over-flow");
		return FALSE;
	}
	CopyMemory((LPVOID)lpShow, lpMem, dwSize);
	for (; i < dwSize; i++)
	{
		printf("%.2X ", lpShow[i]);
		if (!((i + 1) % 16))
		{
			printf("\n");
		}
	}
	printf("\n");
	return TRUE;
}
int main(void)
{
	HANDLE hHeap = GetProcessHeap();
	LPVOID lpSrc;
	LPVOID lpDis;
	lpSrc = HeapAlloc(hHeap, 0, MEM_BLOCK_SIZE);
	lpDis = HeapAlloc(hHeap, 0, MEM_BLOCK_SIZE);
	printf("HeapAlloc 分配但不为零：\n");
	ShowMemContent(lpDis, MEM_BLOCK_SIZE);
	ZeroMemory(lpDis, MEM_BLOCK_SIZE);
	printf("ZeroMemory 清零：\n");
	ShowMemContent(lpDis, MEM_BLOCK_SIZE);
	FillMemory(lpSrc, MEM_BLOCK_SIZE, 0xBB);
	FillMemory(lpSrc, MEM_BLOCK_SIZE / 2, 0xAA);
	CopyMemory(lpDis, lpSrc, MEM_BLOCK_SIZE);
	printf("FillMemory 有规律的填充内存：\n");
	ShowMemContent(lpDis, MEM_BLOCK_SIZE);
	HeapFree(hHeap, 0, lpSrc);
	HeapFree(hHeap, 0, lpDis);
	return 0;
}