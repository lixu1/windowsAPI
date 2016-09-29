#include <Windows.h>
#include <stdio.h>
//内存大小
#define BUFFSIZE 1024
//文件映射的起始位置
#define FILE_MAP_START  0x28804
//文件名
LPTSTR lpcTheFile = TEXT("test.dat");
int main(void)
{
	HANDLE hMapFile;//文件内存映射区域的句柄
	HANDLE hFile;//文件的句柄
	DWORD dBytesWritten;//写入的字节数
	DWORD dwFileSize;//文件大小
	DWORD dwFileMapSize;//文件映射的大小
	DWORD dwMapViewSize;//视图大小
	DWORD dwFileMapStart;//文件映射视图的起始位置
	DWORD dwSysGran;//系统内存分配的粒度
	SYSTEM_INFO SysInfo;//系统信息
	LPVOID lpMapAddress;//内存映射视图的起始位置
	PCHAR pData;//数据
	INT i;
	INT iData;
	INT iViewDelta;
	BYTE cMapBuffer[32];//存储从mapping中 数据
	hFile = CreateFile(lpcTheFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n", GetLastError);
		return 1;
	}
	//依次写入整数，在32位平台下，大小为65535*4字节
	for (i = 0; i < 655535; i++)
	{
		WriteFile(hFile, &i, sizeof(i), &dBytesWritten, NULL);
	}
	dwFileSize = GetFileSize(hFile, NULL); 
	printf("文件大小：%d\n", dwFileSize);
	//获取系统信息，内存分配粒度，为了映射的数据与系统内存分配粒度对齐，提高内存访问效率
	GetSystemInfo(&SysInfo);
	dwSysGran = SysInfo.dwAllocationGranularity;
	//计算mapping的起始位置 
	dwFileMapStart = (FILE_MAP_START / dwSysGran)*dwSysGran;
	//计算mapping view 大小
	dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
	//计算mapping的大小
	dwFileMapSize = FILE_MAP_START + BUFFSIZE;
	//计算需要读取的数据的偏移
	iViewDelta = FILE_MAP_START - dwFileMapStart;
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwFileMapSize, NULL);
	if (hMapFile == NULL)
	{
		printf("CreateFileMapping error:%d\n", GetLastError());
		return 1;
	}
	//映射view
	lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, dwFileMapStart, dwMapViewSize);
	if (lpMapAddress == NULL)
	{
		printf("MapViewOfFile error:%d\n", GetLastError());
		return 1;
	}
	printf("文件map view 相对于文件的起始位置：0x%x\n", dwFileMapStart);
	printf("文件map view的大小：x%x\n", dwMapViewSize);
	printf("文件mapping对象的大小：x%x\n", dwFileMapSize);
	printf("从相对于map view 0x%x 字节的位置读取数据", iViewDelta);
	pData = (PCHAR)lpMapAddress + iViewDelta;
	iData = *(PINT)pData;
	printf("为：x%.8x\n", iData);
	CopyMemory(cMapBuffer, lpMapAddress, 32);
	printf("lpMapAddress起始的字节是：");
	for (i = 0; i < 32; i++)
	{
		printf("0x%.2x ", cMapBuffer[i]);
	}
	//将mapping的前32字节用xff填充
	FillMemory(lpMapAddress, 32, (BYTE)0xff);
	FlushViewOfFile(lpMapAddress, dwMapViewSize);
	printf("已经将lpMapAddress开始的字节使用xff填充。\n");
	if (!CloseHandle(hMapFile))
	{
		printf("closing the mapping object error %d", GetLastError());
	}
	if (!CloseHandle(hFile))
	{
		printf("Error %ld occured closing the file!", GetLastError());
	}
	return 0;

}