#pragma comment(lib, "user32.lib")
#include <stdio.h>
#include <Windows.h>
#define NUMTHREADS 3
#define BUFFER_SIZE 16
#define FOR_TIMES 5
HANDLE hWriteEvent[NUMTHREADS];//write event
HANDLE hReadEvents[NUMTHREADS];//read event
BYTE lpSharedBuffer[16] = { 0 };//share memory
void MultiEvents(void);
VOID WriteToBuffer(VOID);
DWORD WINAPI ThreadFunction(LPVOID lpParam);
int main()
{
	MultiEvents();
}
void MultiEvents(void)
{
	HANDLE hThread;
	DWORD i;
	for (i = 0; i < NUMTHREADS; i++)
	{
		hWriteEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (hWriteEvent[i] == NULL)
		{
			printf("CreateEvent failed (%d)\n", GetLastError());
			return;
		}
		hReadEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (hReadEvents[i] == NULL)
		{
			printf("Create Read Events failed (%d)\n", GetLastError());
			return;
		}
		//create thread
		hThread = CreateThread(NULL, 0, ThreadFunction, (LPVOID)i, 0, NULL);
		if (hThread == NULL)
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return;
		}
	}
	WriteToBuffer();
}
VOID WriteToBuffer(VOID)//write to shared memory, wait all read thread over, and return
{
	DWORD dwWaitResult, j, i;
	for (j = 0; j < FOR_TIMES; j++)
	{
		Sleep(rand() % 100);
		wsprintf((LPSTR)lpSharedBuffer, "shared %d", j);
		printf("set write event\n");
		for (i = 0; i < NUMTHREADS; i++)
		{
			if (!SetEvent(hWriteEvent[i]))
			{
				printf("SetEvent failed (%d)\n", GetLastError());
				return;
			}
		}
		//wait all read thread
		dwWaitResult = WaitForMultipleObjects(NUMTHREADS, hReadEvents, TRUE, INFINITE);
		//wait for result
		if (dwWaitResult != WAIT_OBJECT_0)
		{
			printf("Wait error:%d\n", GetLastError());
			ExitProcess(0);
		}
	}
	Sleep(100);
}
DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	DWORD dwWaitResult;
	BYTE lpRead[16];
	DWORD j = 0;
	DWORD dwThreadIndex = (DWORD)lpParam;
	for (; j < FOR_TIMES; j++)
	{
		//wait write set event
		dwWaitResult = WaitForSingleObject(hWriteEvent[dwThreadIndex], INFINITE);
		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			Sleep(rand() % 10);
			CopyMemory(lpRead, lpSharedBuffer, 16);
			break;
		default:
			printf("Wait error:%d\n", GetLastError());
			ExitThread(0);
		}
		//set read event
		if (!SetEvent(hReadEvents[dwThreadIndex]))
		{
			printf("SetEvent failed (%d)\n", GetLastError());
			return 0;
		}
		printf("线程 %u\t 第%d 次读，内容：%s\n", dwThreadIndex, j, (LPSTR)lpRead);
	}
	return 1;
}