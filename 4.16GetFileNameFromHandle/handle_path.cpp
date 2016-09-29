#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <Psapi.h> 
#pragma comment(lib,"Psapi.lib")
#define BUFSIZE 512
BOOL GetFileNameFromHandle(HANDLE hFile)
{
	TCHAR pszFileName[MAX_PATH + 1];
	HANDLE hFileMap;
	PVOID pMem;
	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
	if (dwFileSizeLow == 0 && dwFileSizeHigh == 0)
	{
		printf("不能map文件大小为0的文件\n");
		return FALSE;
	}
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 1, NULL);
	if (!hFileMap)
	{
		printf("CreateFileMapping error:%d\n", GetLastError());
		return FALSE;
	}
	pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);
	if (!pMem)
	{
		printf("MapViewOfFile error :%d", GetLastError());
		return FALSE;
	}
	if (0 == GetMappedFileName(GetCurrentProcess(), pMem, pszFileName, MAX_PATH))
	{
		printf("GetMappedFileName error:%d\n", GetLastError());
		return FALSE;
	}
	TCHAR szTemp[BUFSIZE] = { 0 };
	if (0 == GetLogicalDriveStrings(BUFSIZE - 1, szTemp))
	{
		printf("GetLogicalDriveStrings error:%d\n", GetLastError());
		return FALSE;
	}
	TCHAR szName[MAX_PATH];
	TCHAR szDrive[3] = { 0 };
	BOOL bFound = FALSE;
	PTCHAR p = szTemp;
	do
	{
		CopyMemory(szDrive, p, 2 * sizeof(TCHAR));
		if (!QueryDosDevice(szDrive, szName, BUFSIZE))
		{
			printf("QueryDosDevice error:%d", GetLastError());
			return FALSE;
		}
		UINT uNameLen = lstrlen(szName);
		if (uNameLen < MAX_PATH)
		{
			bFound = strncmp(pszFileName, szName, uNameLen) == 0;
			if (bFound)
			{
				TCHAR szTempFile[MAX_PATH];
				wsprintf(szTempFile, TEXT("%s%s"), szDrive, pszFileName + uNameLen);
				lstrcpy(pszFileName, szTempFile);
			}
		}
		while (*p++);
	}while (!bFound&&*p);
	UnmapViewOfFile(pMem);
	CloseHandle(hFileMap);
	printf("File path is %s\n", pszFileName);
	return TRUE;
}
int main()
{
	HANDLE hFile;
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	hFind = FindFirstFile("*.txt", &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("can not find a file");
		return 1;
	}
	printf("find %s at current dir\n", wfd.cFileName);
	hFile = CreateFile(wfd.cFileName, GENERIC_READ | GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("create file error,%d", GetLastError());
	}
	else
	{
		GetFileNameFromHandle(hFile);
	}
	CloseHandle(hFile);
	return 0;
}