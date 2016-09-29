#include <Windows.h>
#include <stdio.h>
#define MyAlloc(size) HeapAlloc(GetProcessHeap(),0,size);
#define MyFree(lpMem) HeapFree(GetProcessHeap(),0,lpMem);
typedef struct _PROCESS_INFO
{
	DWORD dwPid;
	HANDLE hProcess;
	DWORD dwProiClass;
	DWORD dwHandleCount;
	DWORD dwAffinityMask;
	SIZE_T dwWorkingSetSizeMax;
	SIZE_T dwWorkingSetSizeMin;
	LPWSTR szwCommandLine;
	STARTUPINFO sti;
}PROCESS_INFO,*LPPROCESS_INFO;
HANDLE hMySelf;
DWORD GetProcessInfo(LPPROCESS_INFO lppi);
int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	PROCESS_INFO pi;
	INT argc;
	WCHAR **argv;
	DWORD i;
	DWORD dwBufferSize = lstrlen(lpCmdLine) + MAX_PATH + 1024;
	LPSTR szShowBuffer = (LPSTR)MyAlloc(dwBufferSize);
	hMySelf = hInstance;
	//winmain function argv
	wsprintf(szShowBuffer, "启动参数\n实例句柄：%.8X,命令行参数：%s,显示选项：%.8X", hInstance, lpCmdLine, nCmdShow);
	MessageBox(NULL, szShowBuffer, "WinMain函数参数", MB_OK);
	GetProcessInfo(&pi);
	argv = CommandLineToArgvW(pi.szwCommandLine, &argc);
	*szShowBuffer = NULL;
	for (i = 0; i < argc; i++)
	{
		DWORD dwBufferSize = lstrlenW(*argv) + 1;
		LPSTR szMBArgv = (LPSTR)MyAlloc(dwBufferSize);
		WideCharToMultiByte(CP_ACP, NULL, *argv, -1, szMBArgv, dwBufferSize, NULL, NULL);
		argv++;
		lstrcat(szShowBuffer, "\n");
		lstrcat(szShowBuffer, szMBArgv);
		MyFree(szShowBuffer);
	}
	MessageBox(NULL, szShowBuffer, "参数", MB_OK);
	MyFree(szShowBuffer);
	return 0;
}
//get process information
DWORD GetProcessInfo(LPPROCESS_INFO lppi)
{
	//pid
	lppi->dwPid = GetCurrentProcessId();
	lppi->hProcess = GetCurrentProcess();
	lppi->dwProiClass = GetPriorityClass(hMySelf);
	GetProcessAffinityMask(hMySelf, &lppi->dwAffinityMask, NULL);
	GetProcessWorkingSetSize(hMySelf, &lppi->dwWorkingSetSizeMin, &lppi->dwWorkingSetSizeMax);
	//setup information
	lppi->szwCommandLine = GetCommandLineW();
	GetStartupInfo(&lppi->sti);
	return 0;
}