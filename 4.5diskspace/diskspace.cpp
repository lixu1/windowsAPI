#include <windows.h>
#include <stdio.h>
BOOL GetDiskSpaceInfo(LPCSTR pszDrive)
{
	DWORD64 qwFreeBytesToCaller, qwTotalBytes, qwFreeBytes;
	DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
	BOOL bResult;
	bResult = GetDiskFreeSpaceEx(pszDrive,
		(PULARGE_INTEGER)&qwFreeBytesToCaller,
		(PULARGE_INTEGER)&qwTotalBytes,
		(PULARGE_INTEGER)&qwFreeBytes);
	if (bResult)
	{
		printf("使用GetDiskFreeSpaceEx获取磁盘空间信息\n");
		printf("可获得的空闲空间（字节）：\t\t%I64d\n", qwFreeBytesToCaller);
		printf("空闲空间（字节）：\t\t%I64d\n", qwFreeBytes);
		printf("磁盘总容量(字节):\t\t%I64d\n", qwTotalBytes);
	}
	bResult = GetDiskFreeSpace(pszDrive, &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters);
	if (bResult)
	{
		printf("使用GetDiskFreeSpace获取磁盘空间信息\n");
		printf("空闲的簇数量：\t\t%d\n", dwFreeClusters);
		printf("总簇数量：\t\t%d\n", dwTotalClusters);
		printf("每扇区的容量(字节)：\t\t%d\n", dwBytesPerSect);
		printf("空闲空间（字节）：\t\t%I64d\n", (DWORD64)dwFreeClusters*(DWORD64)dwSectPerClust
			*(DWORD64)dwBytesPerSect);
		printf("磁盘总数量（字节）：\t\t%I64d", (DWORD64)dwTotalClusters*(DWORD64)dwSectPerClust
			*(DWORD64)dwBytesPerSect);

	}
	return bResult;
}
int main(int argc, PCHAR argv[])
{
	GetDiskSpaceInfo(argv[1]);
}