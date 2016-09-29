#include <Windows.h>
#include <stdio.h>
DWORD ShowFileTime(PFILETIME lptime)
{
	FILETIME ftLocal;
	SYSTEMTIME st;
	FileTimeToLocalFileTime(lptime, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &st);
	printf("%4d��%#02d��%#02d�գ�%#02d:%#02d:%#02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return 0;
}
DWORD ShowFileSize(DWORD dwFileSizeHigh, DWORD dwFileSizeLow)
{
	ULONGLONG liFileSize;
	liFileSize = dwFileSizeHigh;
	liFileSize <<= sizeof(DWORD) * 8;
	liFileSize += dwFileSizeLow;
	printf("�ļ���С��\t%I64u�ֽ�\n", liFileSize);
	return 0;
}
DWORD ShowFileAttrInfo(DWORD dwAttribute)
{
	printf("�ļ����ԣ�\t");
	if (dwAttribute&FILE_ATTRIBUTE_ARCHIVE)
		printf("<ARCHIVE>");
	if (dwAttribute&FILE_ATTRIBUTE_COMPRESSED)
		printf("<ѹ��>");
	if (dwAttribute&FILE_ATTRIBUTE_DIRECTORY)
		printf("<Ŀ¼>");
	if (dwAttribute&FILE_ATTRIBUTE_ENCRYPTED)
		printf("<����>");
	if (dwAttribute&FILE_ATTRIBUTE_HIDDEN)
		printf("<����>");
	if (dwAttribute&FILE_ATTRIBUTE_NORMAL)
		printf("<NORMAL>");
	if (dwAttribute&FILE_ATTRIBUTE_OFFLINE)
		printf("<OFFLINE>");
	if (dwAttribute&FILE_ATTRIBUTE_READONLY)
		printf("<ֻ��>");
	if (dwAttribute&FILE_ATTRIBUTE_SPARSE_FILE)
		printf("<SPARSE>");
	if (dwAttribute&FILE_ATTRIBUTE_SYSTEM)
		printf("<ϵͳ�ļ�>");
	if (dwAttribute&FILE_ATTRIBUTE_TEMPORARY)
		printf("<��ʱ�ļ�>");
	printf("\n");
	return 0;
}
DWORD ShowFileAttributes(LPSTR szPath)
{
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	printf("�ļ���%s\n", szPath);
	if (!GetFileAttributesEx(szPath, GetFileExInfoStandard, &wfad))
	{
		printf("��ȡ�ļ����Դ���%d\n", GetLastError());
		return 1;
	}
	printf("����ʱ�䣺\t");
	ShowFileTime(&(wfad.ftCreationTime));
	printf("������ʱ�䣺\t");
	ShowFileTime(&(wfad.ftLastAccessTime));
	printf("����޸�ʱ�䣺\t");
	ShowFileTime(&(wfad.ftLastWriteTime));
	ShowFileSize(wfad.nFileSizeHigh, wfad.nFileSizeLow);
	ShowFileAttrInfo(wfad.dwFileAttributes);
	return 0;
}
DWORD SetFileHiddenAndReadonly(LPSTR szFileName)
{
	DWORD dwFileAttributes = GetFileAttributes(szFileName);
	dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
	dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
	if (SetFileAttributes(szFileName, dwFileAttributes))
		printf("�ļ�%s�����غ��������óɹ�\n", szFileName);
	else
		printf("�������ã�%d\n", GetLastError());
	return 0;
}
int main(int argc, PCHAR argv[])
{
	if (argc != 3)
	{
		printf("���������\n");
		printf("��ʾ��һ������ָ���ļ������ԡ�ʱ�䡢��С��\n");
		printf("���ڶ�����������������Ϊ���ء�ֻ��");
		return 1;
	}
	ShowFileAttributes(argv[1]);
	SetFileHiddenAndReadonly(argv[2]);
	return 0;
}