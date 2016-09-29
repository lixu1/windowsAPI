#define _WIN32_WINNT 0x0501
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
//�оٹ��ص�
BOOL ProcessVolumeMountPoint(HANDLE hPt, TCHAR *PtBuf, DWORD dwPtBufSize, TCHAR *Buf)
{
	BOOL bFlag;//���
	TCHAR Path[BUFSIZE];//ȫ·��
	TCHAR Target[BUFSIZE];//���ص��豸
	printf("\tVolume mount point found is \" %s \"\n", PtBuf);
	lstrcpy(Path, Buf);
	lstrcat(Path, PtBuf);
	bFlag = GetVolumeNameForVolumeMountPoint(Path, Target, BUFSIZE);
	if (!bFlag)
	{
		printf("\tAttempt to get volume name for %s failed.\n", Path);
	}
	else
		printf("\tTarget of the volume mount point is %s.\n", Target);
	bFlag = FindNextVolumeMountPoint(hPt, PtBuf, dwPtBufSize);
	return bFlag;
}
//�жϾ����ͣ��оٹ��ص�
BOOL ProcessVolume(HANDLE hVol, TCHAR *Buf, DWORD iBufSize)
{
	BOOL bFlag;//���ر�־
	HANDLE hPt;//����
	TCHAR PtBuf[BUFSIZE];//���ص�·��
	DWORD dwSysFlags;//�ļ�ϵͳ���
	TCHAR FileSysNameBuf[FILESYSNAMEBUFSIZE];
	printf("Volume found is \"%s\".\n", Buf);
	//whether it is ntfs
	GetVolumeInformation(Buf, NULL, 0, NULL, NULL, &dwSysFlags, FileSysNameBuf, FILESYSNAMEBUFSIZE);
	if (!(dwSysFlags&FILE_SUPPORTS_REPARSE_POINTS))
	{
		printf("\tThis file system does not support volume mount points.\n");
	}
	else
	{
		//�����еĹ��ص�
		hPt = FindFirstVolumeMountPoint(
			Buf,
			PtBuf,
			BUFSIZE);
		if (hPt == INVALID_HANDLE_VALUE)
		{
			printf("\tNo volume mount points found!\n");
		}
		else
		{
			//������ص�
			bFlag = ProcessVolumeMountPoint(hPt, PtBuf, BUFSIZE, Buf);
			while (bFlag)
				bFlag = ProcessVolumeMountPoint(hPt, PtBuf, BUFSIZE, Buf);
			FindVolumeMountPointClose(hPt);
		}
	}
	bFlag = FindNextVolume(hVol, Buf, iBufSize);
	return bFlag;
}
//��ȡ���ص�
int GetMountPoint(void)
{
	TCHAR buf[BUFSIZE];
	HANDLE hVol;
	BOOL bFlag;
	printf("Volume mount points info of this computer:\n\n");
	hVol = FindFirstVolume(buf, BUFSIZE);
	if (hVol = INVALID_HANDLE_VALUE)
	{
		printf("No volumes found\n");
		return -1;
	}
	bFlag = ProcessVolume(hVol, buf, BUFSIZE);
	while (bFlag)
	{
		bFlag = ProcessVolume(hVol, buf, BUFSIZE);
	}
	bFlag = FindVolumeClose(hVol);
	return bFlag;
}
//ʹ�÷���
void Usage(PCHAR argv)
{
	printf("\n\n\t%s,mount a volume at a mount point.\n", argv);
	printf("\tFor example, \"mount D:\\mnt\drives\\ E:\\\"\n");
}
int main(int argc, PCHAR argv[])
{
	BOOL bFlag;
	CHAR Buf[BUFSIZE];
	if (argc != 3)
	{
		GetMountPoint();
		Usage(argv[0]);
		return -1;
	}
	bFlag = GetVolumeNameForVolumeMountPoint(argv[2], Buf, BUFSIZE);
	if (bFlag != TRUE)
	{
		printf("Retrieving volume name for %s failed.\n", argv[2]);
		return -2;
	}
	printf("Volume name of %s is %s\n", argv[2], Buf);
	bFlag = SetVolumeMountPoint(argv[1], Buf);
	if (!bFlag)
	{
		printf("Attempt to mount %s at %s failed. error code is \n", argv[2], argv[1], GetLastError());
	}
	return bFlag;
}