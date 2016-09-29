#include <Windows.h>
#include <stdio.h>
DWORD ReadFileContent(LPSTR szFilePath)
{
	HANDLE hFileRead;
	LARGE_INTEGER liFileSize;
	DWORD dwReadedSize;
	LONGLONG liTotalRead = 0;
	BYTE lpFileDataBuffer[32];
	hFileRead = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFileRead == INVALID_HANDLE_VALUE)
	{
		printf("���ļ�ʧ�ܣ�%d", GetLastError());
	}
	if (!GetFileSizeEx(hFileRead, &liFileSize))
	{
		printf("��ȡ�ļ���Сʧ�ܣ�%d", GetLastError());
	}
	else
	{
		printf("�ļ���С��%d\n", liFileSize.QuadPart);
	}
	while (TRUE)
	{
		DWORD i;
		if (!ReadFile(hFileRead, lpFileDataBuffer, 32, &dwReadedSize, NULL))
		{
			printf("���ļ�ʧ�ܣ�%d\n", GetLastError());
			break;
		}
		printf("��ȡ��%d�ֽڣ��ļ������ǣ�", dwReadedSize);
		for (i = 0; i < dwReadedSize; i++)
		{
			printf("0x%x ", lpFileDataBuffer[i]);
		}
		printf("\n");
		liTotalRead += dwReadedSize;
		if (liTotalRead == liFileSize.QuadPart)
		{
			printf("���ļ�����");
			break;
		}
	}
	CloseHandle(hFileRead);
	return 0;
}
DWORD SaveDataToFile(LPSTR szFilePath, LPVOID lpData, DWORD dwDataSize)
{
	HANDLE hFileWrite;
	DWORD dwWritedDataSize;
	hFileWrite = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFilePointer(hFileWrite, 0, 0, FILE_END);
	if (!WriteFile(hFileWrite, lpData, dwDataSize, &dwWritedDataSize, NULL))
		printf("д�ļ�ʧ�ܣ�%d\n", GetLastError());
	else
		printf("д�ļ��ɹ���д��%d�ֽ�\n", dwWritedDataSize);
	CloseHandle(hFileWrite);
	return 0;
}
int main(void)
{
	LPSTR szFileData = "����һ������";
	SaveDataToFile("H:\\test.txt", szFileData, lstrlen(szFileData));
	return 0;

}