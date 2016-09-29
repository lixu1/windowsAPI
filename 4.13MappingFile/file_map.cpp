#include <Windows.h>
#include <stdio.h>
//�ڴ��С
#define BUFFSIZE 1024
//�ļ�ӳ�����ʼλ��
#define FILE_MAP_START  0x28804
//�ļ���
LPTSTR lpcTheFile = TEXT("test.dat");
int main(void)
{
	HANDLE hMapFile;//�ļ��ڴ�ӳ������ľ��
	HANDLE hFile;//�ļ��ľ��
	DWORD dBytesWritten;//д����ֽ���
	DWORD dwFileSize;//�ļ���С
	DWORD dwFileMapSize;//�ļ�ӳ��Ĵ�С
	DWORD dwMapViewSize;//��ͼ��С
	DWORD dwFileMapStart;//�ļ�ӳ����ͼ����ʼλ��
	DWORD dwSysGran;//ϵͳ�ڴ���������
	SYSTEM_INFO SysInfo;//ϵͳ��Ϣ
	LPVOID lpMapAddress;//�ڴ�ӳ����ͼ����ʼλ��
	PCHAR pData;//����
	INT i;
	INT iData;
	INT iViewDelta;
	BYTE cMapBuffer[32];//�洢��mapping�� ����
	hFile = CreateFile(lpcTheFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n", GetLastError);
		return 1;
	}
	//����д����������32λƽ̨�£���СΪ65535*4�ֽ�
	for (i = 0; i < 655535; i++)
	{
		WriteFile(hFile, &i, sizeof(i), &dBytesWritten, NULL);
	}
	dwFileSize = GetFileSize(hFile, NULL); 
	printf("�ļ���С��%d\n", dwFileSize);
	//��ȡϵͳ��Ϣ���ڴ�������ȣ�Ϊ��ӳ���������ϵͳ�ڴ�������ȶ��룬����ڴ����Ч��
	GetSystemInfo(&SysInfo);
	dwSysGran = SysInfo.dwAllocationGranularity;
	//����mapping����ʼλ�� 
	dwFileMapStart = (FILE_MAP_START / dwSysGran)*dwSysGran;
	//����mapping view ��С
	dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
	//����mapping�Ĵ�С
	dwFileMapSize = FILE_MAP_START + BUFFSIZE;
	//������Ҫ��ȡ�����ݵ�ƫ��
	iViewDelta = FILE_MAP_START - dwFileMapStart;
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwFileMapSize, NULL);
	if (hMapFile == NULL)
	{
		printf("CreateFileMapping error:%d\n", GetLastError());
		return 1;
	}
	//ӳ��view
	lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, dwFileMapStart, dwMapViewSize);
	if (lpMapAddress == NULL)
	{
		printf("MapViewOfFile error:%d\n", GetLastError());
		return 1;
	}
	printf("�ļ�map view ������ļ�����ʼλ�ã�0x%x\n", dwFileMapStart);
	printf("�ļ�map view�Ĵ�С��x%x\n", dwMapViewSize);
	printf("�ļ�mapping����Ĵ�С��x%x\n", dwFileMapSize);
	printf("�������map view 0x%x �ֽڵ�λ�ö�ȡ����", iViewDelta);
	pData = (PCHAR)lpMapAddress + iViewDelta;
	iData = *(PINT)pData;
	printf("Ϊ��x%.8x\n", iData);
	CopyMemory(cMapBuffer, lpMapAddress, 32);
	printf("lpMapAddress��ʼ���ֽ��ǣ�");
	for (i = 0; i < 32; i++)
	{
		printf("0x%.2x ", cMapBuffer[i]);
	}
	//��mapping��ǰ32�ֽ���xff���
	FillMemory(lpMapAddress, 32, (BYTE)0xff);
	FlushViewOfFile(lpMapAddress, dwMapViewSize);
	printf("�Ѿ���lpMapAddress��ʼ���ֽ�ʹ��xff��䡣\n");
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