#include <Windows.h>
#include <stdio.h>
int main(int argc, PCHAR argv[])
{
	printf("ʹ��˵����\nɾ���ļ�:\t-d �ļ�·��\n�����ļ�:\t-c �ļ�·��1 �ļ�·��2\n�ƶ��ļ�:\t-m �ļ�·��1 �ļ�·��2\n");
	if (0 == lstrcmp("-d", argv[1]) && argc == 3)
	{
		if (!DeleteFile(argv[2]))
			printf("ɾ���ļ�����%x\n", GetLastError());
		else
			printf("ɾ���ɹ�\n");
	}
	else if (0 == lstrcmp("-c", argv[1]) && argc == 4)
	{
		if (!CopyFile(argv[2], argv[3], TRUE))
		{
			if (GetLastError() == 0x50)
			{
				printf("�ļ�%s�Ѿ������Ƿ񸲸ǣ�y/n:", argv[3]);
				if ('y' == getchar())
				{
					if (!CopyFile(argv[2], argv[3], FALSE))
						printf("�����ļ�����,%d\n", GetLastError());
					else
						printf("���Ƴɹ�\n");
				}
				else
					return 0;
			}
		}
		else
			printf("���Ƴɹ�\n");
	}
	else if (0 == lstrcmp("-m", argv[1]) && argc == 4)
	{
		if (!MoveFile(argv[2], argv[3]))
		{
			printf("�ƶ��ļ�����:%d\n", GetLastError());
		}
		else
			printf("�ƶ��ļ��ɹ�\n");
	}
	else
		printf("��������\n");
}