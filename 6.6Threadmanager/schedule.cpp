#include <Windows.h>
#include <stdio.h>
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	LPDWORD pData;
	DWORD i = 0;
	pData = (LPDWORD)lpParam;
	for (i; i < 10; i++)
	{
		Sleep(100);
		printf("TID=%u,\tParameters=%u\ti=%u\n", GetCurrentThreadId(), *pData, i);
	}
	ExitThread(i);
	return 0;
}
void main()
{
	DWORD dwData;
	DWORD dwThreadId[2];
	HANDLE hThread[2];
	//create thread
	dwData = 1;
	hThread[0] = CreateThread(NULL, 0, ThreadProc, &dwData, CREATE_SUSPENDED, &dwThreadId[0]);//suspend thread
	if (hThread[0] == NULL)
	{
		ExitProcess(0);
	}
	//create thread
	dwData = 2;
	hThread[1] = CreateThread(NULL, 0, ThreadProc, &dwData, 0, &dwThreadId[1]);
	if (hThread[1] == NULL)
	{
		ExitProcess(1);
	}
	//wait 200ms thread1 execute 1 time
	Sleep(200);
	ResumeThread(hThread[0]);
	//suspend thread 1
	SuspendThread(hThread[1]);
	//wait 300 ms; thread0 execute 2 times
	Sleep(300);
	//thread0 over
	TerminateThread(hThread[0], 0);
	//thread1 resume will execute 9 times
	ResumeThread(hThread[1]);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
}