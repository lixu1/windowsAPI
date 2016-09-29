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
		printf("打开文件失败：%d", GetLastError());
	}
	if (!GetFileSizeEx(hFileRead, &liFileSize))
	{
		printf("获取文件大小失败：%d", GetLastError());
	}
	else
	{
		printf("文件大小：%d\n", liFileSize.QuadPart);
	}
	while (TRUE)
	{
		DWORD i;
		if (!ReadFile(hFileRead, lpFileDataBuffer, 32, &dwReadedSize, NULL))
		{
			printf("读文件失败：%d\n", GetLastError());
			break;
		}
		printf("读取了%d字节，文件内容是：", dwReadedSize);
		for (i = 0; i < dwReadedSize; i++)
		{
			printf("0x%x ", lpFileDataBuffer[i]);
		}
		printf("\n");
		liTotalRead += dwReadedSize;
		if (liTotalRead == liFileSize.QuadPart)
		{
			printf("读文件结束");
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
		printf("写文件失败：%d\n", GetLastError());
	else
		printf("写文件成功，写入%d字节\n", dwWritedDataSize);
	CloseHandle(hFileWrite);
	return 0;
}
int main(void)
{
	LPSTR szFileData = "这是一个例子";
	SaveDataToFile("H:\\test.txt", szFileData, lstrlen(szFileData));
	return 0;

}