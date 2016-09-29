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
	//关闭连接实例
	if (!DisconnectNamedPipe(lpPipeInst->hPipeInst))
	{
		printf("DisconnectNamedPipe failed with %d\n", GetLastError());
	}
	//关闭pipe实例的句柄
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
//写入pipe操作的完成函数，当写操作完成时被调用，开始读另外一个请求
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
	//出错，断开连接
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
	//用于连接操作的事件对象
	hConnectEvent = CreateEvent(NULL, //默认属性
		TRUE, //手工reset
		TRUE,//初始状态signaled
		NULL);//未命名
	if (hConnectEvent == NULL)
	{
		printf("CreateEvent failed with %d\n", GetLastError());
		return 0;
	}
	oConnect.hEvent = hConnectEvent;
	//创建连接实例，等待连接
	fPendingIO = CreateAndConnectInstance(&oConnect);
	while (1)
	{
		//等待客户端连接或读写操作完成
		dwWait = WaitForSingleObjectEx(hConnectEvent, //等待的事件
			INFINITE, //无限等待
			TRUE);
		switch (dwWait)
		{
		case 0:
			if (fPendingIO)
			{
				fSuccess = GetOverlappedResult(hPipe,//pipe句柄 
					&oConnect, //OVERLAPPED 结构
					&cbRet, //已经传送的数据量
					FALSE);//不等待
				if (!fSuccess)
				{
					printf("connectNamedPipe (%d) \n", GetLastError());
					return 0;
				}
			}
			//分配内存
			lpPipeInst = (LPPIPEINST)HeapAlloc(GetProcessHeap(), 0, sizeof(PIPEINST));
			if (lpPipeInst == NULL)
			{
				printf("GlobalAlloc failed (%d)\n", GetLastError());
				return 0;
			}
			lpPipeInst->hPipeInst = hPipe;
			lpPipeInst->cbToWrite = 0;
			CompletedWriteRoutine(0, 0, (LPOVERLAPPED)lpPipeInst);
			//再创建一个连接实例，以响应下一个客户端的连接
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