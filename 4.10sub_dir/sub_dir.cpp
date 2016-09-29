#include <Windows.h>
#include <stdio.h>
DWORD EnumerateFileInDirectory(LPSTR szPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	CHAR szFilePath[MAX_PATH];
	lstrcpy(szFilePath, szPath);
	lstrcat(szFilePath, "\\*");
	hListFile = FindFirstFile(szFilePath, &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE)
	{
		printf("´íÎó:%d\n", GetLastError());
		return 1;
	}
	else
	{
		do
		{
			printf("%s\t\t", FindFileData.cFileName);
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_ENCRYPTED)
			{
				printf("<¼ÓÃÜ>");
			}
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)
			{
				printf("<Òþ²Ø>");
			}
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("<DIR>");
			}
			printf("\n");
		} while (FindNextFile(hListFile, &FindFileData));
	}
	return 0;
}
int main(int argc, PCHAR argv[])
{
	if (argc == 2)
	{
		EnumerateFileInDirectory(argv[1]);
	}
	else
	{
		CHAR szCurrentPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, szCurrentPath);
		EnumerateFileInDirectory(szCurrentPath);
	}
	return 0;
}