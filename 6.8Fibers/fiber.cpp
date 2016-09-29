#define _WIN32_WINNT 0x0501
#include <Windows.h>
#include <stdio.h>
VOID WINAPI ReadFiberFunc(LPVOID lpParameter);
VOID WINAPI WriteFiberFunc(LPVOID lpParameter);
typedef struct _FIBERDATASTRUCT
{
	DWORD dwParameter;//parameter to thread
	DWORD dwFiberResultCode;//getlasterror
	HANDLE hFile;//fiber control file handle
	DWORD dwBytesProcessed;//bytes had been processed
}FIBERDATASTRUCT,*LPFIBERDATASTRUCT;
#define RTN_OK 0//success
#define RTN_USAGE 1//error parameter
#define RTN_ERROR 2//error
#define BUFFER_SIZE 32768//buffer size
#define FIBER_COUNT 3//main fiber; read fiber; write fiber
#define PRIMARY_FIBER 0//main fiber
#define READ_FIBER 1//read fiber
#define WRITE_FIBER 2//write fiber

LPVOID g_lpFiber[FIBER_COUNT];
LPBYTE g_lpBuffer;
DWORD g_dwBytesRead;
int main(int argc, char *argv[])
{
	LPFIBERDATASTRUCT fs;
	if (argc != 3)
	{
		printf("Usage: %s <SourceFile> <DestinationFile>\n", argv[0]);
		return RTN_USAGE;
	}
	//allocate fiberdatastruct 3 numbers
	fs = (LPFIBERDATASTRUCT)HeapAlloc(GetProcessHeap(), 0, sizeof(FIBERDATASTRUCT)*FIBER_COUNT);
	if (fs == NULL)
	{
		printf("HeapAlloc error (rc%=1u)\n", GetLastError());
		return RTN_ERROR;
	}
	g_lpBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, BUFFER_SIZE);
	if (g_lpBuffer == NULL)
	{
		printf("HeapAlloc error (rc=%1u)", GetLastError());
		return RTN_ERROR;
	}
	fs[READ_FIBER].hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fs[READ_FIBER].hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile READ_FIBER  error! (rc=%1u)\n", GetLastError());
		return RTN_ERROR;
	}
	fs[WRITE_FIBER].hFile = CreateFile(argv[2], GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fs[WRITE_FIBER].hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile WRITE_FIBER error! (rc=%1u)\n", GetLastError());
		return RTN_ERROR;
	}
	g_lpFiber[PRIMARY_FIBER] = ConvertThreadToFiber(&fs[PRIMARY_FIBER]);
	if (g_lpFiber[PRIMARY_FIBER] == NULL)
	{
		printf("ConvertThreadToFiber failed! rc=%1u\n", GetLastError());
		return RTN_ERROR;
	}
	//primary fiber data
	fs[PRIMARY_FIBER].dwParameter = 0;
	fs[PRIMARY_FIBER].dwFiberResultCode = 0;
	fs[PRIMARY_FIBER].hFile = INVALID_HANDLE_VALUE;
	//create read fiber
	g_lpFiber[READ_FIBER] = CreateFiber(0, ReadFiberFunc, &fs[READ_FIBER]);
	if (g_lpFiber[READ_FIBER] == NULL)
	{
		printf("CreateFiber error! (rc=%1u)\n", GetLastError());
		return RTN_ERROR;
	}
	fs[READ_FIBER].dwParameter =(DWORD) g_lpFiber[READ_FIBER];
	//create write fiber
	g_lpFiber[WRITE_FIBER] = CreateFiber(0, WriteFiberFunc, &fs[WRITE_FIBER]);
	if (g_lpFiber[WRITE_FIBER] == NULL)
	{
		printf("CreateFiber error! (rc=%1u)\n", GetLastError());
		return RTN_ERROR;
	}
	fs[WRITE_FIBER].dwParameter = (DWORD)g_lpFiber[WRITE_FIBER];
	//switch to read fiber
	SwitchToFiber(g_lpFiber[READ_FIBER]);
	//show some info
	printf("ReadFiber result == %1u Bytes Processed == %1u\n",
		fs[READ_FIBER].dwFiberResultCode, fs[READ_FIBER].dwBytesProcessed);
	printf("WriteFiber result == %1u Bytes Processed == %1u\n",
		fs[WRITE_FIBER].dwFiberResultCode, fs[WRITE_FIBER].dwBytesProcessed);
	//delete read write fiber
	DeleteFiber(g_lpFiber[READ_FIBER]);
	DeleteFiber(g_lpFiber[WRITE_FIBER]);
	//close handle
	CloseHandle(fs[READ_FIBER].hFile);
	CloseHandle(fs[WRITE_FIBER].hFile);
	HeapFree(GetProcessHeap(), 0, g_lpBuffer);
	HeapFree(GetProcessHeap(), 0, fs);
	return RTN_OK;
}
VOID WINAPI ReadFiberFunc(LPVOID lpParameter)
{
	LPFIBERDATASTRUCT fds = (LPFIBERDATASTRUCT)lpParameter;
	if (fds == NULL)
	{
		printf("Passed NULL fiber data. Existing current thread.\n");
		return;
	}
	printf("Read Fiber (dwParameter == 0x%1x)\n", fds->dwParameter);
	fds->dwBytesProcessed = 0;
	while (1)
	{
		if (!ReadFile(fds->hFile, g_lpBuffer, BUFFER_SIZE, &g_dwBytesRead, NULL))
			break;
		if (g_dwBytesRead == 0)
			break;
		fds->dwBytesProcessed += g_dwBytesRead;
		printf("Switch to Write");
		SwitchToFiber(g_lpFiber[WRITE_FIBER]);
	}
	fds->dwFiberResultCode = GetLastError();
	SwitchToFiber(g_lpFiber[PRIMARY_FIBER]);
}

void WINAPI WriteFiberFunc(LPVOID lpParameter)
{
	LPFIBERDATASTRUCT fds = (LPFIBERDATASTRUCT)lpParameter;
	DWORD dwBytesWritten;
	if (fds == NULL)
	{
		printf("Passed NULL fiber data.Existing current thread.\n");
		return;
	}
	printf("Write Fiber (dwparameter == 0x%1x)\n", fds->dwParameter);
	fds->dwBytesProcessed = 0;
	fds->dwFiberResultCode = ERROR_SUCCESS;
	while (1)
	{
		if (!WriteFile(fds->hFile, g_lpBuffer, g_dwBytesRead, &dwBytesWritten, NULL))
		{
			break;
		}
		fds->dwBytesProcessed += dwBytesWritten;
		printf("Switch To Read");
		SwitchToFiber(g_lpFiber[READ_FIBER]);
	}
	fds->dwFiberResultCode = GetLastError();
	SwitchToFiber(g_lpFiber[PRIMARY_FIBER]);
}