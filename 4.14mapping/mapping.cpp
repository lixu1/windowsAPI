#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#define BUF_SIZE 256
LPSTR szName = TEXT("SharedFileMappingObject");
LPSTR szMsg = TEXT("进程的消息");
void main(int argc, PCHAR argv[])
{
	HANDLE hMapFile;
	LPTSTR pBuf;
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, szName);
	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFileMapping error:%d\n", GetLastError());
		return;
	}
	pBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	if (pBuf == NULL)
	{
		printf("MapViewOfFile error %d\n", GetLastError());
		return;
	}
	if (argc == 1)
	{
		CopyMemory((PVOID)pBuf, szMsg, strlen(szMsg));
	}
	else
	{
		DWORD dwCopyLen = (strlen(argv[1]) < BUF_SIZE) ? strlen(argv[1]) : BUF_SIZE;
		CopyMemory((PVOID)pBuf, argv[1], dwCopyLen);
	}
	printf("运行程序，完成运行时，按任意键退出");
	_getch();
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}