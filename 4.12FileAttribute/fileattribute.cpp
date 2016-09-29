#include <Windows.h>
#include <stdio.h>
DWORD ShowFileTime(PFILETIME lptime)
{
	FILETIME ftLocal;
	SYSTEMTIME st;
	FileTimeToLocalFileTime(lptime, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &st);
	printf("%4d年%#02d月%#02d日，%#02d:%#02d:%#02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return 0;
}
DWORD ShowFileSize(DWORD dwFileSizeHigh, DWORD dwFileSizeLow)
{
	ULONGLONG liFileSize;
	liFileSize = dwFileSizeHigh;
	liFileSize <<= sizeof(DWORD) * 8;
	liFileSize += dwFileSizeLow;
	printf("文件大小：\t%I64u字节\n", liFileSize);
	return 0;
}
DWORD ShowFileAttrInfo(DWORD dwAttribute)
{
	printf("文件属性：\t");
	if (dwAttribute&FILE_ATTRIBUTE_ARCHIVE)
		printf("<ARCHIVE>");
	if (dwAttribute&FILE_ATTRIBUTE_COMPRESSED)
		printf("<压缩>");
	if (dwAttribute&FILE_ATTRIBUTE_DIRECTORY)
		printf("<目录>");
	if (dwAttribute&FILE_ATTRIBUTE_ENCRYPTED)
		printf("<加密>");
	if (dwAttribute&FILE_ATTRIBUTE_HIDDEN)
		printf("<隐藏>");
	if (dwAttribute&FILE_ATTRIBUTE_NORMAL)
		printf("<NORMAL>");
	if (dwAttribute&FILE_ATTRIBUTE_OFFLINE)
		printf("<OFFLINE>");
	if (dwAttribute&FILE_ATTRIBUTE_READONLY)
		printf("<只读>");
	if (dwAttribute&FILE_ATTRIBUTE_SPARSE_FILE)
		printf("<SPARSE>");
	if (dwAttribute&FILE_ATTRIBUTE_SYSTEM)
		printf("<系统文件>");
	if (dwAttribute&FILE_ATTRIBUTE_TEMPORARY)
		printf("<临时文件>");
	printf("\n");
	return 0;
}
DWORD ShowFileAttributes(LPSTR szPath)
{
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	printf("文件：%s\n", szPath);
	if (!GetFileAttributesEx(szPath, GetFileExInfoStandard, &wfad))
	{
		printf("获取文件属性错误：%d\n", GetLastError());
		return 1;
	}
	printf("创建时间：\t");
	ShowFileTime(&(wfad.ftCreationTime));
	printf("最后访问时间：\t");
	ShowFileTime(&(wfad.ftLastAccessTime));
	printf("最后修改时间：\t");
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
		printf("文件%s的隐藏和属性设置成功\n", szFileName);
	else
		printf("属性设置：%d\n", GetLastError());
	return 0;
}
int main(int argc, PCHAR argv[])
{
	if (argc != 3)
	{
		printf("请输入参数\n");
		printf("显示第一个参数指定文件的属性、时间、大小；\n");
		printf("将第二个参数的属性设置为隐藏、只读");
		return 1;
	}
	ShowFileAttributes(argv[1]);
	SetFileHiddenAndReadonly(argv[2]);
	return 0;
}