#include <Windows.h>
#include <stdio.h>
int main(void)
{
	//�洢��ǰ·��
	CHAR szCurrentDirectory[MAX_PATH];
	//�洢ģ��·��
	CHAR szMoudlePath[MAX_PATH];
	//kernel32�ļ����;��
	LPSTR szKernel32 = "kernel32.dll";
	HMODULE hKernel32;
	//��ǰ·���ĳ��ȣ�Ҳ�����жϻ�ȡ�Ƿ�ɹ�
	DWORD dwCurDirPathLen;
	dwCurDirPathLen = GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	if (dwCurDirPathLen == 0)
	{
		printf("��ȡ��ǰĿ¼����");
		return 0;
	}
	printf("���̵�ǰĿ¼Ϊ%s\n", szCurrentDirectory);
	lstrcpy(szCurrentDirectory, "H:\\");
	if (!SetCurrentDirectory(szCurrentDirectory))
	{
		printf("���õ�ǰĿ¼����");
		return 0;
	}
	printf("�Ѿ����õ�ǰĿ¼Ϊ%s\n", szCurrentDirectory);
	//ʹ�����·������ʼ��Ϊ��������ʱ�ĵ�ǰĿ¼�������ǿ�ִ���ļ�����Ŀ¼
	CreateDirectory("current_dir", NULL);
	dwCurDirPathLen = GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	if (dwCurDirPathLen == 0)
	{
		printf("��ȡ��ǰĿ¼����");
		return 0;
	}
	printf("GetCurrentDirectory��ȡ��ǰĿ¼Ϊ%s\n", szCurrentDirectory);
	if (!GetModuleFileName(NULL, szMoudlePath, MAX_PATH))
	{
		printf("��ȡģ��·������");
		return 0;
	}
	printf("��ģ��·��%s\n", szMoudlePath);
	hKernel32 = LoadLibrary(szKernel32);
	if (!GetModuleFileName(hKernel32, szMoudlePath, MAX_PATH))
	{
		printf("��ȡģ��·������");
		return 0;
	}
	printf("kernel32ģ��·�� %s\n", szMoudlePath);
	return 0;
}