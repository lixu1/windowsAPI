#include <Windows.h>
#include <stdio.h>
HANDLE hSlot;
LPTSTR lpszSlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
LPTSTR lpszMessage = TEXT("Test Message for mailslot");
//mailslot communication client
void main()
{
	BOOL fResult;
	HANDLE hFile;
	DWORD cbWritten;
	DWORD cbMessage;
	hFile = CreateFile(lpszSlotName, 
		GENERIC_WRITE, //可写
		FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,//打开一个已经存在的mailslot，应该有服务端创建
		FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d.\n", GetLastError());
		return;
	}
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	lpszMessage = sys.wHour + ':' + sys.wMinute + ':' + sys.wSecond + lpszMessage;
	//printf("%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	fResult = WriteFile(hFile, lpszMessage, (DWORD)(lstrlen(lpszMessage) + 1)*sizeof(TCHAR), &cbWritten, (LPOVERLAPPED)NULL);
	if (!fResult)
	{
		printf("WriteFile failed with %d\n", GetLastError());
		return;
	}
	printf("Slot written to successfully\n");
	CloseHandle(hFile);
	return;
}