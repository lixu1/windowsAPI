#include <Windows.h>
#include <stdio.h>
int main(void)
{
	LPSTR szDirPath = "sub_dir";
	if (!CreateDirectory(szDirPath, NULL))
	{
		printf("创建目录%s错误\n", szDirPath);
		return 1;
	}
	szDirPath = "C:\\example_dir";
	if (!CreateDirectory(szDirPath, NULL))
	{
		printf("创建目录%s错误\n", szDirPath);
		return 1;
	}
	printf("成功\n");
	return 0;
}