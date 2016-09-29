#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#pragma comment(lib,"User32.lib")
#define BUF_SIZE 256
TCHAR szName[] = TEXT("SharedFileMappingObject");
void main()
{
	HANDLE hMapFile;
	LPTSTR pBuf;
	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);
	if (hMapFile == NULL)
	{
		printf("OpenFileMapping error:%d\n", GetLastError());
		return;
	}
	pBuf =(LPTSTR) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	if (pBuf == NULL)
	{
		printf("MapViewOfFile error %d\n", GetLastError());
		return;
	}
	MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);  
}