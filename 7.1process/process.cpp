#include <Windows.h>
#include <stdio.h>

DWORD CreateChildProcess(LPSTR szChildProcessCmd)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//clear startup information
	ZeroMemory(&si, sizeof(si));
	//set struct's size
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(NULL, szChildProcessCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		//create process fail
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 1;
	}
	//father process wait child process 
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}
int main()
{
	CreateChildProcess("6.2child.exe abc 123");
}