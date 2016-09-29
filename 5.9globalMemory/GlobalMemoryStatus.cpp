#include <Windows.h>
#include <stdio.h>
int main(void)
{
	MEMORYSTATUSEX memstatusex;
	memstatusex.dwLength = sizeof(memstatusex);
	GlobalMemoryStatusEx(&memstatusex);
	printf("TotalPhys\tAvailPhys\tTotalVirtual\tAvailVirtual\tTotalPageFile\tuAvailPageFile\tMemoryLoad\n"
		"%I64u\t%I64u\t%I64u\t%I64u\t%I64u\t%I64u\t%u%%",
		memstatusex.ullTotalPhys, memstatusex.ullAvailVirtual,
		memstatusex.ullTotalVirtual, memstatusex.ullAvailVirtual,
		memstatusex.ullTotalPageFile, memstatusex.ullAvailPageFile,
		memstatusex.dwMemoryLoad);
	return 0;
} 