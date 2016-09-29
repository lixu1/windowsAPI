#include <Windows.h>
#include <stdio.h>
DWORD PrintHeapSize(HANDLE hHeap,LPVOID lpMem)
{
	SIZE_T dwHeapSize;
	dwHeapSize = HeapSize(hHeap, HEAP_NO_SERIALIZE, lpMem);
	if (dwHeapSize == -1)
	{
		printf("Get HeapSize error:%d", GetLastError());
		return 1;
	}
	printf("内存块大小为:0x%x\n", dwHeapSize);
	return 0;
}
int main(int argc, PCHAR argv[])
{
	SYSTEM_INFO si;
	HANDLE hHeap;
	LPVOID lpMem;
	LPVOID lpReAlloc;
	DWORD dwHeapSize;
	HANDLE hHeaps[24];
	DWORD dwHeapNum;
	GetSystemInfo(&si);
	printf("系统内存页大小：0x%x\n系统内存分配粒度：0x%x\n", si.dwPageSize, si.dwAllocationGranularity);
	if (argc == 2 && 0 == lstrcmp(argv[1], "-a"))
	{
		hHeap = HeapCreate(HEAP_NO_SERIALIZE, si.dwPageSize, si.dwPageSize * 10);
		printf("创建堆，初始化大小为1页，最大为10页\n");
	}
	else if (argc == 2 && 0 == lstrcmp(argv[1], "-s"))
	{
		hHeap = GetProcessHeap();
		printf("获取系统存在的堆\n");
	}
	else
	{
		hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
		printf("创建堆，初始化大小为1页，大小可变");
	}
	if (hHeap == NULL)
	{
		printf("创建或获取进程堆错误：%d", GetLastError());
		return 1;
	}
	//获取将打印当前进程中一共有多少个堆，有没有新建堆
	dwHeapNum = GetProcessHeaps(24, hHeaps);
	if (dwHeapNum == 0)
	{
		printf("GetProcessHeaps error:%d", GetLastError());
	}
	else
	{
		printf("当前进程一共有%u个堆",dwHeapNum);
	}
	lpMem = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, si.dwPageSize * 3);
	if (lpMem == NULL)
	{
		printf("HeapAlloc error:%u", GetLastError());
		return 1;
	}
	printf("在堆上成功分配内存，起始地址为：0x%x\n", lpMem);
	PrintHeapSize(hHeap, lpMem);
	lpReAlloc = HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, lpMem, si.dwPageSize * 11);
	if (lpReAlloc == NULL)
	{
		printf("HeapReALLco error:%d", GetLastError());
		return 1;
	}
	printf("在堆上再分配内存，地址为0x%x,原地址为0x%x\n", lpReAlloc, lpMem);
	PrintHeapSize(hHeap, lpReAlloc);
	if (!HeapFree(hHeap, HEAP_NO_SERIALIZE, lpReAlloc))
	{
		printf("HeapFree error:%d", GetLastError());
		return 1;
	}
	printf("释放内存成功\n");
	if (argc != 2 || 0 != lstrcmp(argv[1], "-s"))
	{
		printf("销毁HeapCreate创建的堆\n");
		if (!HeapDestroy(hHeap))
		{
			printf("HeapDestory error:%d", GetLastError());
			return 1;
		}
		printf("销毁堆成功");
	}
	return 0;
}