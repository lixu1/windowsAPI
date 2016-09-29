#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
HANDLE hPipe;
#define BUFSIZE 4096
#define PIPE_TIMEOUT 5000
typedef struct
{
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;
	TCHAR chRequest[BUFSIZE];
	DWORD cbRead;
	TCHAR chReply[BUFSIZE];
	DWORD cbToWrite;
}PIPEINST,*LPPIPEINST;
VOID DisconnectAndClose(LPPIPEINST);
BOOL CreateAndConnectInstance(LPOVERLAPPED);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
VOID GetAnswerToRequest(LPPIPEINST);
VOID WINAPI CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED);
VOID WINAPI CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED);

VOID DisconnectAndClose(LPPIPEINST lpPipeInst)
{
	//�ر�����ʵ��
	if (!DisconnectNamedPipe(lpPipeInst->hPipeInst))
	{
		printf("DisconnectNamedPipe failed with %d\n", GetLastError());
	}
	//�ر�pipeʵ���ľ��
	CloseHandle(lpPipeInst->hPipeInst);
	if (lpPipeInst != NULL)
		HeapFree(GetProcessHeap(), 0, lpPipeInst);
}

VOID GetAnswerToRequest(LPPIPEINST pipe)
{
	_tprintf(TEXT("[%d] %s\n"), pipe->hPipeInst, pipe->chRequest);
	lstrcpyn(pipe->chReply, pipe->chRequest  /* TEXT("Default answer from server")*/, BUFSIZE);
	pipe->cbToWrite = (lstrlen(pipe->chReply) + 1)*sizeof(TCHAR);
}
BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = FALSE;
	fConnected = ConnectNamedPipe(hPipe, lpo);
	if (fConnected)
	{
		printf("ConnectNamedPipe failed with %d\n", GetLastError());
		return 0;
	}
	switch (GetLastError())
	{
	case ERROR_IO_PENDING:
		fPendingIO = TRUE;
		break;
	case ERROR_PIPE_CONNECTED:
		if (SetEvent(lpo->hEvent))
			break;
	default:
	{
		printf("ConnectNamedPipe failed with %d\n", GetLastError());
		return 0;
	}
	}
	return fPendingIO;
}
BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap)
{
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\samplenamepipe");
	hPipe = CreateNamedPipe(lpszPipename, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
		BUFSIZE*sizeof(TCHAR), BUFSIZE*sizeof(TCHAR), PIPE_TIMEOUT, NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe failed with %d\n", GetLastError());
		return 0;
	}
	return ConnectToNewClient(hPipe, lpoOverlap);
}
VOID WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead, LPOVERLAPPED lpOverLap)
{
	LPPIPEINST lpPipeInst;
	BOOL fWrite = FALSE;
	lpPipeInst = (LPPIPEINST)lpOverLap;
	if ((dwErr == 0) && (cbBytesRead != 0))
	{
		GetAnswerToRequest(lpPipeInst);
		fWrite = WriteFileEx(lpPipeInst->hPipeInst, lpPipeInst->chReply, lpPipeInst->cbToWrite, (LPOVERLAPPED)lpPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedWriteRoutine);
	}
	if (!fWrite)
		DisconnectAndClose(lpPipeInst);
}
//д��pipe��������ɺ�������д�������ʱ�����ã���ʼ������һ������
VOID WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cdWritten, LPOVERLAPPED lpOverLap)
{
	LPPIPEINST lpPipeInst;
	BOOL fRead = FALSE;
	lpPipeInst = (LPPIPEINST)lpOverLap;
	if ((dwErr == 0)&&(cdWritten == lpPipeInst->cbToWrite))
	{
		fRead = ReadFileEx(lpPipeInst->hPipeInst, lpPipeInst->chRequest, BUFSIZE*sizeof(TCHAR), (LPOVERLAPPED)lpPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedReadRoutine);
	}
	//�����Ͽ�����
	if (!fRead)
		DisconnectAndClose(lpPipeInst);
}
int main(VOID)
{
	HANDLE hConnectEvent;
	OVERLAPPED oConnect;
	LPPIPEINST lpPipeInst;
	DWORD dwWait, cbRet;
	BOOL fSuccess, fPendingIO;
	//�������Ӳ������¼�����
	hConnectEvent = CreateEvent(NULL, //Ĭ������
		TRUE, //�ֹ�reset
		TRUE,//��ʼ״̬signaled
		NULL);//δ����
	if (hConnectEvent == NULL)
	{
		printf("CreateEvent failed with %d\n", GetLastError());
		return 0;
	}
	oConnect.hEvent = hConnectEvent;
	//��������ʵ�����ȴ�����
	fPendingIO = CreateAndConnectInstance(&oConnect);
	while (1)
	{
		//�ȴ��ͻ������ӻ��д�������
		dwWait = WaitForSingleObjectEx(hConnectEvent, //�ȴ����¼�
			INFINITE, //���޵ȴ�
			TRUE);
		switch (dwWait)
		{
		case 0:
			if (fPendingIO)
			{
				fSuccess = GetOverlappedResult(hPipe,//pipe��� 
					&oConnect, //OVERLAPPED �ṹ
					&cbRet, //�Ѿ����͵�������
					FALSE);//���ȴ�
				if (!fSuccess)
				{
					printf("connectNamedPipe (%d) \n", GetLastError());
					return 0;
				}
			}
			//�����ڴ�
			lpPipeInst = (LPPIPEINST)HeapAlloc(GetProcessHeap(), 0, sizeof(PIPEINST));
			if (lpPipeInst == NULL)
			{
				printf("GlobalAlloc failed (%d)\n", GetLastError());
				return 0;
			}
			lpPipeInst->hPipeInst = hPipe;
			lpPipeInst->cbToWrite = 0;
			CompletedWriteRoutine(0, 0, (LPOVERLAPPED)lpPipeInst);
			//�ٴ���һ������ʵ��������Ӧ��һ���ͻ��˵�����
			fPendingIO = CreateAndConnectInstance(&oConnect);
			break;
		case WAIT_IO_COMPLETION:
			break;
		default:
		{
			printf("WaitForSingleObjectEx (%d)\n", GetLastError());
			return 0;
		}
		}
	}
}