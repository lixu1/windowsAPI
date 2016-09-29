#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#define BUFSIZE 512
int main(int argc, TCHAR *argv[])
{
	HANDLE hPipe;
	LPTSTR lpvMessage = TEXT("Default message from client");
	TCHAR chBuf[BUFSIZE];
	BOOL fSuccess;
	DWORD cbRead, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\samplenamepipe");
	if (argc > 1)
		lpvMessage = argv[1];
	while (1)
	{
		hPipe = CreateFile(lpszPipename,//pipe名
			GENERIC_READ | GENERIC_WRITE,//可读可写
			0, //不共享
			NULL,//默认安全属性
			OPEN_EXISTING,//已经存在（由服务器创建）
			0, //默认属性
			NULL);
		if (hPipe != INVALID_HANDLE_VALUE)
			break;
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			printf("Could not open pipe");
			return 0;
		}
		if (!WaitNamedPipe(lpszPipename, 2000))
		{
			printf("Cound not open pipe");
			return 0;
		}
	}
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);
	if (!fSuccess)
	{
		printf("SetNamedPipeHandleState failed");
		return 0;
	}
	fSuccess = WriteFile(hPipe, lpvMessage, (lstrlen(lpvMessage) + 1)*sizeof(TCHAR), &cbWritten, NULL);
	if (!fSuccess)
	{
		printf("WriteFile failed");
		return 0;
	}
	do
	{
		fSuccess = ReadFile(hPipe, chBuf, BUFSIZE*sizeof(TCHAR), &cbRead, NULL);
		if (!fSuccess&&GetLastError() != ERROR_MORE_DATA)
			break;
		_tprintf(TEXT("%s\n"), chBuf);
	} while (!fSuccess);
	_getch();
	CloseHandle(hPipe);
	return 0;
}