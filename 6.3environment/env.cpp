#include <Windows.h>
#include <stdio.h>
#include "env.h"
#define BUFSIZE 4096
DWORD WINAPI EnumEnvironmentVariables()
{
	printf("\n\n");
	//get the process all environment variables
	PVOID pEv = GetEnvironmentStrings();
	LPSTR szEnvs;
	for (szEnvs = (LPSTR)pEv; *szEnvs;)
	{
		printf("%s\n", szEnvs);
		while (*szEnvs++);
	}
	FreeEnvironmentStrings((LPCH)pEv);
	printf("\n\n");
	return 0;
}
DWORD WINAPI ChangeEnvironmentVariables(LPSTR szName, LPSTR szNewValue, DWORD dwFlag)
{
	DWORD dwErr;
	PVOID szVal;
	DWORD dwReturn;
	DWORD dwNewValSize;
	if (dwFlag == VARIABLES_APPEND)
	{
		//size of new variable
		dwNewValSize = lstrlen(szNewValue) + 1;
		//allocate memory
		szVal = HeapAlloc(GetProcessHeap(), 0, BUFSIZE + dwNewValSize);
		dwReturn = GetEnvironmentVariable(szName, (LPSTR)szVal, BUFSIZE);
		if (dwReturn == 0)
		{
			dwErr = GetLastError();
			if (ERROR_ENVVAR_NOT_FOUND == dwErr)
			{
				printf("Environmenr variable %s does not exist.\n", szName);
			}
			else
			{
				printf("error:%d", dwErr);
			}
			return FALSE;
		}
		else if (BUFSIZE < dwReturn)//buffer size is too small
		{
			szVal = (LPTSTR)HeapReAlloc(GetProcessHeap(), 0, szVal, dwReturn + dwNewValSize);
			if (szVal == NULL)
			{
				printf("Memory error\n");
				return FALSE;
			}
		}
		dwReturn = GetEnvironmentVariable(szName, (LPSTR)szVal, dwReturn);
		if (!dwReturn)
		{
			printf("GetEnvironmentVariable failed (%d)\n", GetLastError());
			return FALSE;
		}
		lstrcat((LPSTR)szVal, ";");
		lstrcat((LPSTR)szVal, szNewValue);
		if (!SetEnvironmentVariable(szName,(LPCSTR)szVal))
		{
			printf("Set value Error %d", GetLastError());
		}
		HeapFree(GetProcessHeap(), 0, szVal);
		return TRUE;
	}
	else if (dwFlag == VARIABLES_RESET)
	{
		if (!SetEnvironmentVariable(szName, szNewValue))
		{
			printf("Set value error %d", GetLastError());
		}
	}
	else if (dwFlag == VARIABLES_NULL)
	{
		if (!SetEnvironmentVariable(szName, NULL))
		{
			printf("Set value error %d", GetLastError());
		}
	}
	return TRUE;
}
int main(void)
{
	EnumEnvironmentVariables();
	ChangeEnvironmentVariables("PATH", "C:\\", VARIABLES_APPEND);
	EnumEnvironmentVariables();
	ChangeEnvironmentVariables("PATH", "C:\\", VARIABLES_NULL);
	EnumEnvironmentVariables();
	ChangeEnvironmentVariables("XX", "C:\\", VARIABLES_RESET);
	EnumEnvironmentVariables();
}