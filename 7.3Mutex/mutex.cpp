#include <Windows.h>
#include <stdio.h>
#define NUM_THREADS 4
#define MUTEX
DWORD dwCounter = 0;
HANDLE hMutex;
void UseMutex(void);
DWORD WINAPI MutexThread(LPVOID lpParam);
int main()
{
	UseMutex(); 
}
void UseMutex(void)
{
	INT i;
	HANDLE hThread;
#ifdef MUTEX
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
	}
#endif
	for (i = 0; i < NUM_THREADS; i++)
	{
		hThread = CreateThread(NULL, 0, MutexThread, NULL, 0, NULL);
		if (hThread == NULL)
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return;
		}
	}
	Sleep(1000);
}
DWORD WINAPI MutexThread(LPVOID lpParam)
{
#ifdef MUTEX
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(hMutex, INFINITE);
	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
#endif
		Sleep(rand() % 100);
		printf("counter:%d\n", dwCounter);
		dwCounter++;
#ifdef MUTEX
		if (!ReleaseMutex(hMutex))
		{
			printf("Release Mutex error: %d\n", GetLastError());
		}
		break;
	default:
		printf("Wait error:%d\n", GetLastError());
		ExitThread(0);
}
#endif
		return 1;
}