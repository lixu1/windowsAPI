#include <Windows.h>
#include <stdio.h>
int main(void)
{
	LPSTR szDirPath = "sub_dir";
	if (!CreateDirectory(szDirPath, NULL))
	{
		printf("����Ŀ¼%s����\n", szDirPath);
		return 1;
	}
	szDirPath = "C:\\example_dir";
	if (!CreateDirectory(szDirPath, NULL))
	{
		printf("����Ŀ¼%s����\n", szDirPath);
		return 1;
	}
	printf("�ɹ�\n");
	return 0;
}