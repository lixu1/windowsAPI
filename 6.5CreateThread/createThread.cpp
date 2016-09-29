#include <Windows.h>
#include <stdio.h>
#define MAX_THREADS 5
typedef struct _THREAD_PARAM{
	DWORD i;
	DWORD dwRandom;
	DWORD dwData;
}THREAD_PARAM,*LPTHREAD_PARAM;
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	LPTHREAD_PARAM pData;
	pData = (LPTHREAD_PARAM)lpParam;
	printf("TID=%u,\tParameters=%u,%u,%u\n", GetCurrentThreadId(), pData->i, pData->dwRandom, pData->dwData);
	HeapFree(GetProcessHeap(), 0, pData);
	return 0;
}
void main()
{
	LPTHREAD_PARAM pData;
	DWORD dwThreadId[MAX_THREADS];
	HANDLE hThread[MAX_THREADS];
	int i;
	for (i = 0; i < MAX_THREADS; i++)
	{
		pData = (LPTHREAD_PARAM)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(THREAD_PARAM));
		if (pData == NULL)
		{
			printf("HeapAlloc error\n");
			ExitProcess(2);
		}
		pData->i = i;//number 
		pData->dwRandom = rand();//random
		pData->dwData = 100;//data 100
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, pData, 0, &dwThreadId[i]);
		if (hThread[i] == NULL)
		{
			printf("CreateThread error\n");
			ExitProcess(2);
		}
	}
	//thread end sequence random
	WaitForMultipleObjects(MAX_THREADS, hThread, TRUE, INFINITE);
	for (i = 0; i < MAX_THREADS; i++)
	{
		CloseHandle(hThread[i]);
	}
}