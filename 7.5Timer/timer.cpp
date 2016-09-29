#define _WIN32_WINNT 0x0500
#include <Windows.h>
#include <stdio.h>
#define ONE_SECOND 10000000
typedef struct _APC_PROC_ARG{
	TCHAR *szText;
	DWORD dwValue;
}APC_PROC_ARG;
VOID CALLBACK TimerAPCProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	APC_PROC_ARG *pApcData = (APC_PROC_ARG *)lpArg;
	printf("Message:%s\nvalue:%d\n", pApcData->szText, pApcData->dwValue);
	//MessageBeep(MB_OK);
}
void main(void)
{
	HANDLE hTimer;
	BOOL bSuccess;
	INT64 qwDueTime;
	LARGE_INTEGER liDueTime;
	APC_PROC_ARG ApcData;
	ApcData.szText = "Message to apc proc.";
	ApcData.dwValue = 1;
	hTimer = CreateWaitableTimer(NULL, FALSE, "MyTimer");
	if (!hTimer)
	{
		printf("CreateWaitableTimer failed with Error %d.",GetLastError());
		return;
	}
	else
	{
		//try
		//{
			//5s
			qwDueTime = -5 * ONE_SECOND;
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);
			bSuccess = SetWaitableTimer(hTimer, &liDueTime, 1000, TimerAPCProc, &ApcData, FALSE);
			if (bSuccess)
			{
				for (; ApcData.dwValue < 10; ApcData.dwValue++)
				{
					SleepEx(INFINITE, TRUE);
				}
			}
			else
			{
				printf("SetWaitableTimer failed with Error %d.", GetLastError());
			}
		//}
		//catch ()
		//{

		//}
		//_finally
		//{
			CloseHandle(hTimer);
		//}
	}
}