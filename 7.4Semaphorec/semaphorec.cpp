#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#define NUMTHREADS 4
HANDLE hSemaphore;
void UseSemaphore(void);
DWORD WINAPI SemaphoreThread(LPVOID lpParam);
int main()
{
	UseSemaphore();
}
DWORD WaitForAllThread(HANDLE hThread[], DWORD dwNumThread)
{
	DWORD dwWaitResult = WaitForMultipleObjects(dwNumThread, hThread, TRUE, INFINITE);
	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		printf("\nAll thread exit\n");
		break;
	default:
		printf("\nWait error:%u", GetLastError());
	}
	return 0;
}
void UseSemaphore(void)
{
	HANDLE hThread[NUMTHREADS];
	INT i;
	LONG lMax;
	CHAR cMax;
	printf("将创建%d个进程，获得信号量的进程可以向屏幕打印\n请输入信号量的最大计数1~%d:", NUMTHREADS, NUMTHREADS);
	cMax = _getch();
	printf("%c\n", cMax);
	lMax = cMax & 0xF;
	if (lMax<0 || lMax>NUMTHREADS)
	{
		printf("请输入1-%d", NUMTHREADS);
	}
	hSemaphore = CreateSemaphore(NULL, lMax, lMax, NULL);
	if (hSemaphore == NULL)
	{
		printf("CreateSemaphore error:%d\n", GetLastError());
	}
	for (i = 0; i < NUMTHREADS; i++)
	{
		hThread[i] = CreateThread(NULL, 0, SemaphoreThread, &i, 0, NULL);
		if (hThread[i] == NULL)
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return;
		}
	}
	WaitForAllThread(hThread, NUMTHREADS);
}
//thread function read shared memory
DWORD WINAPI SemaphoreThread(LPVOID lpParam)
{
	DWORD dwWaitResult;
	BYTE lpRead[16];
	DWORD dwPreviousCount;
	DWORD j = 0;
	for (; j < 3; j++)
	{
		Sleep(rand() % 1000);
		dwWaitResult = WaitForSingleObject(hSemaphore,INFINITE);
		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			printf("\nProcess %d Gets Semaphore", GetCurrentThreadId());
			break;
		default:
			printf("\nProcess %u Wait error:%u", GetCurrentThreadId(), GetLastError());
		}
		Sleep(rand() % 1000);
		if (!ReleaseSemaphore(hSemaphore, 1, (LPLONG)&dwPreviousCount))
		{
			printf("\nProcess %u releaseSemaphore error:%d", GetCurrentThreadId(), GetLastError());
		}
		else
		{
			printf("\nProcess %u release Semaphore,previous count is %u", GetCurrentThreadId(), dwPreviousCount);
		}
	}
	return 1;
}