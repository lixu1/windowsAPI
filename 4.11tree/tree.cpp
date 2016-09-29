#include <Windows.h>
#include <stdio.h>
#pragma comment(lib,"User32.lib")
DWORD ListAllFileInDirectory(LPSTR szPath);
DWORD dwTotalFileNum = 0;
DWORD ListAllFileInDirectory(LPSTR szPath)
{
	CHAR szFilePath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	CHAR szFullPath[MAX_PATH];
	lstrcpy(szFilePath, szPath);
	lstrcat(szFilePath, "\\*");
	hListFile = FindFirstFile(szFilePath, &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE)
	{
		printf("´íÎó£º%d\n", GetLastError());
		return 1;
	}
	else
	{
		do
		{
			if (lstrcmp(FindFileData.cFileName, TEXT(".")) == 0
				|| lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
			{
				continue;
			}
			wsprintf(szFullPath, "%s\\%s", szPath, FindFileData.cFileName);
			dwTotalFileNum++;
			printf("\n%d\t%s\t", dwTotalFileNum, szFullPath);
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("<DIR>");
				ListAllFileInDirectory(szFullPath);
			}
		} while (FindNextFile(hListFile, &FindFileData));
	}
	return 0;
}
int main(int argc, PCHAR argv[])
{
	if (argc == 2)
	{
		ListAllFileInDirectory(argv[1]);
	}
	else
	{
		CHAR szCurrentPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, szCurrentPath);
		ListAllFileInDirectory(szCurrentPath);
	}
	return 0;
}