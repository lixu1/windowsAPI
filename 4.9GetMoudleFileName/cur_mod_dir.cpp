#include <Windows.h>
#include <stdio.h>
int main(void)
{
	//存储当前路径
	CHAR szCurrentDirectory[MAX_PATH];
	//存储模块路径
	CHAR szMoudlePath[MAX_PATH];
	//kernel32文件名和句柄
	LPSTR szKernel32 = "kernel32.dll";
	HMODULE hKernel32;
	//当前路径的长度，也可以判断获取是否成功
	DWORD dwCurDirPathLen;
	dwCurDirPathLen = GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	if (dwCurDirPathLen == 0)
	{
		printf("获取当前目录错误");
		return 0;
	}
	printf("进程当前目录为%s\n", szCurrentDirectory);
	lstrcpy(szCurrentDirectory, "H:\\");
	if (!SetCurrentDirectory(szCurrentDirectory))
	{
		printf("设置当前目录错误");
		return 0;
	}
	printf("已经设置当前目录为%s\n", szCurrentDirectory);
	//使用相对路径，起始点为程序运行时的当前目录，而不是可执行文件所在目录
	CreateDirectory("current_dir", NULL);
	dwCurDirPathLen = GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	if (dwCurDirPathLen == 0)
	{
		printf("获取当前目录错误");
		return 0;
	}
	printf("GetCurrentDirectory获取当前目录为%s\n", szCurrentDirectory);
	if (!GetModuleFileName(NULL, szMoudlePath, MAX_PATH))
	{
		printf("获取模块路径错误");
		return 0;
	}
	printf("本模块路径%s\n", szMoudlePath);
	hKernel32 = LoadLibrary(szKernel32);
	if (!GetModuleFileName(hKernel32, szMoudlePath, MAX_PATH))
	{
		printf("获取模块路径错误");
		return 0;
	}
	printf("kernel32模块路径 %s\n", szMoudlePath);
	return 0;
}