#include <Windows.h>
#include <stdio.h>
#define NUMTHREADS 3
#define BUFFER_SIZE 16
#define FOR_TIMES 5
HANDLE hEvent;//for Synchronous
BYTE lpSharedBuffer[16] = { 0 };//share memory
void UseEvents(void);
DWORD WINAPI EventFunction(LPVOID lpParam);
int main()
{
	UseEvents();
}
void UseEvents(void)
{
	HANDLE hThread;
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return;
	}
	hThread = CreateThread(NULL, 0, EventFunction, NULL, 0, NULL);
	if (hThread==NULL)
	{
		printf("CreateThread failed (%d)\n", GetLastError());
		return;
	}
	Sleep(2000);
	CopyMemory(lpSharedBuffer, "Event", lstrlen("Event"));
	SetEvent(hEvent);
	Sleep(2000);
}
DWORD WINAPI EventFunction(LPVOID lpParam)
{
	printf("-----\n");
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(hEvent, INFINITE);
	if (dwWaitResult != WAIT_OBJECT_0)
	{
		printf("Wait error: %d\n", GetLastError());
		return 0;
	}
	printf("-----\n");
	printf((char*)lpSharedBuffer);
	if (!ResetEvent(hEvent))
	{
		printf("SetEvent failed (%d)\n", GetLastError());
		return 0;
	}
	return 1;
}