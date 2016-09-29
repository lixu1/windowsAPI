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
		printf("ʹ��GetDiskFreeSpaceEx��ȡ���̿ռ���Ϣ\n");
		printf("�ɻ�õĿ��пռ䣨�ֽڣ���\t\t%I64d\n", qwFreeBytesToCaller);
		printf("���пռ䣨�ֽڣ���\t\t%I64d\n", qwFreeBytes);
		printf("����������(�ֽ�):\t\t%I64d\n", qwTotalBytes);
	}
	bResult = GetDiskFreeSpace(pszDrive, &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters);
	if (bResult)
	{
		printf("ʹ��GetDiskFreeSpace��ȡ���̿ռ���Ϣ\n");
		printf("���еĴ�������\t\t%d\n", dwFreeClusters);
		printf("�ܴ�������\t\t%d\n", dwTotalClusters);
		printf("ÿ����������(�ֽ�)��\t\t%d\n", dwBytesPerSect);
		printf("���пռ䣨�ֽڣ���\t\t%I64d\n", (DWORD64)dwFreeClusters*(DWORD64)dwSectPerClust
			*(DWORD64)dwBytesPerSect);
		printf("�������������ֽڣ���\t\t%I64d", (DWORD64)dwTotalClusters*(DWORD64)dwSectPerClust
			*(DWORD64)dwBytesPerSect);

	}
	return bResult;
}
int main(int argc, PCHAR argv[])
{
	GetDiskSpaceInfo(argv[1]);
}