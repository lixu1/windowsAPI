#include <Windows.h>
#include <stdio.h>
int main(int argc, PCHAR argv[])
{
	printf("使用说明：\n删除文件:\t-d 文件路径\n复制文件:\t-c 文件路径1 文件路径2\n移动文件:\t-m 文件路劲1 文件路径2\n");
	if (0 == lstrcmp("-d", argv[1]) && argc == 3)
	{
		if (!DeleteFile(argv[2]))
			printf("删除文件错误：%x\n", GetLastError());
		else
			printf("删除成功\n");
	}
	else if (0 == lstrcmp("-c", argv[1]) && argc == 4)
	{
		if (!CopyFile(argv[2], argv[3], TRUE))
		{
			if (GetLastError() == 0x50)
			{
				printf("文件%s已经存在是否覆盖？y/n:", argv[3]);
				if ('y' == getchar())
				{
					if (!CopyFile(argv[2], argv[3], FALSE))
						printf("复制文件错误,%d\n", GetLastError());
					else
						printf("复制成功\n");
				}
				else
					return 0;
			}
		}
		else
			printf("复制成功\n");
	}
	else if (0 == lstrcmp("-m", argv[1]) && argc == 4)
	{
		if (!MoveFile(argv[2], argv[3]))
		{
			printf("移动文件错误:%d\n", GetLastError());
		}
		else
			printf("移动文件成功\n");
	}
	else
		printf("参数错误\n");
}