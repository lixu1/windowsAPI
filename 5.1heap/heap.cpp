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
	printf("�ڴ���СΪ:0x%x\n", dwHeapSize);
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
	printf("ϵͳ�ڴ�ҳ��С��0x%x\nϵͳ�ڴ�������ȣ�0x%x\n", si.dwPageSize, si.dwAllocationGranularity);
	if (argc == 2 && 0 == lstrcmp(argv[1], "-a"))
	{
		hHeap = HeapCreate(HEAP_NO_SERIALIZE, si.dwPageSize, si.dwPageSize * 10);
		printf("�����ѣ���ʼ����СΪ1ҳ�����Ϊ10ҳ\n");
	}
	else if (argc == 2 && 0 == lstrcmp(argv[1], "-s"))
	{
		hHeap = GetProcessHeap();
		printf("��ȡϵͳ���ڵĶ�\n");
	}
	else
	{
		hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
		printf("�����ѣ���ʼ����СΪ1ҳ����С�ɱ�");
	}
	if (hHeap == NULL)
	{
		printf("�������ȡ���̶Ѵ���%d", GetLastError());
		return 1;
	}
	//��ȡ����ӡ��ǰ������һ���ж��ٸ��ѣ���û���½���
	dwHeapNum = GetProcessHeaps(24, hHeaps);
	if (dwHeapNum == 0)
	{
		printf("GetProcessHeaps error:%d", GetLastError());
	}
	else
	{
		printf("��ǰ����һ����%u����",dwHeapNum);
	}
	lpMem = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, si.dwPageSize * 3);
	if (lpMem == NULL)
	{
		printf("HeapAlloc error:%u", GetLastError());
		return 1;
	}
	printf("�ڶ��ϳɹ������ڴ棬��ʼ��ַΪ��0x%x\n", lpMem);
	PrintHeapSize(hHeap, lpMem);
	lpReAlloc = HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, lpMem, si.dwPageSize * 11);
	if (lpReAlloc == NULL)
	{
		printf("HeapReALLco error:%d", GetLastError());
		return 1;
	}
	printf("�ڶ����ٷ����ڴ棬��ַΪ0x%x,ԭ��ַΪ0x%x\n", lpReAlloc, lpMem);
	PrintHeapSize(hHeap, lpReAlloc);
	if (!HeapFree(hHeap, HEAP_NO_SERIALIZE, lpReAlloc))
	{
		printf("HeapFree error:%d", GetLastError());
		return 1;
	}
	printf("�ͷ��ڴ�ɹ�\n");
	if (argc != 2 || 0 != lstrcmp(argv[1], "-s"))
	{
		printf("����HeapCreate�����Ķ�\n");
		if (!HeapDestroy(hHeap))
		{
			printf("HeapDestory error:%d", GetLastError());
			return 1;
		}
		printf("���ٶѳɹ�");
	}
	return 0;
}