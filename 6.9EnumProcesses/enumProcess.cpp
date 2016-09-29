#include <Windows.h>
#include <stdio.h>
VOID ListProcessModules1(DWORD dwPID)
{
	HMOUDLE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;
	printf("\nListProcessModules1 Process ID %u\n", dwPID);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	if (NULL == hProcess)
		return;
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(hMOUDLE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				printf(TEXT("\t%s(0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}
	CloseHandle(hProcess);
}
VOID ListProcessModules2(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	printf("\nListProcessModules2 Process ID %u\n", dwPID);

}
VOID WINAPI EnumProcess1()
{
	//assume no more that 1024 processes
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (!EnumProcesses(aProcesses, sizeof(aProcess), &cbNeeded)
		return;
	cProcesses = cbNeeded / sizeof£¨DWORD);
	for (i = 0; i < cProcesses; i++)
	{
		printf("\n\n******************");
		printf("\nPROCESS:%u\n\n", aProcesses[i]);
		printf("\n********************");
		ListProcessModules1(aProcess[i]);
		ListProcessThreads[aProcess[i]];
	}
}
VOID WINAPI EnumProcess2()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		PrintError("CreateToolhelp32Snapshot(of processes)");
		return (FALSE);
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		PrintError("Process32First");
		CloseHandle(hProcessSnap);
		return (FALSE);
	}
	do
	{
		printf(":");
	}
}