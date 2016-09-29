#include <Windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")

HANDLE hSlot;
LPTSTR lpszSlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
LPTSTR Message = TEXT("Message for mailslot in primary domain.");
//mailslot communication server
void main()
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	LPTSTR lpszBuffer;
	TCHAR achID[80];
	DWORD cAllMessages;
	HANDLE hEvent;
	OVERLAPPED ov;
	cbMessage = cbMessage = cbRead = 0;
	hSlot = CreateMailslot(lpszSlotName,//mailslot名 
		0, //不限制消息大小
		MAILSLOT_WAIT_FOREVER,//无超时
		(LPSECURITY_ATTRIBUTES)NULL);
	if (hSlot == INVALID_HANDLE_VALUE)
	{
		printf("Create Mailslot failed with %d\n", GetLastError());
		return;
	}
	else printf("Mailslot created successfully\n");
	while (1)
	{
		//获取mailslot消息
		fResult = GetMailslotInfo(hSlot, //句柄
			(LPDWORD)NULL, //无最大消息限制
			&cbMessage, //下一条消息大小
			&cMessage, //消息数量
			(LPDWORD)NULL);//无时限
		if (!fResult)
		{
			printf("GetMailslotInfo failed with %d.\n", GetLastError());
			return;
		}
		if (cbMessage == MAILSLOT_NO_MESSAGE)
		{
			//没有消息，过一段时间再去读
			Sleep(20000);
			continue;
		}
		cAllMessages = cMessage;
		while (cMessage != 0)
		{
			wsprintf((LPTSTR)achID, "\nMessage #%d of %d\n", cAllMessages - cMessage + 1, cAllMessages);
			//分配空间
			lpszBuffer = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lstrlen((LPTSTR)achID)*sizeof(TCHAR) + cbMessage);
			if (NULL == lpszBuffer)
			{
				return;
			}
			//读取消息
			fResult = ReadFile(hSlot, //句柄
				lpszBuffer, //缓存
				cbMessage, //消息长度
				&cbRead, //实际读取长度
				NULL);
			if (!fResult)
			{
				printf("ReadFile failed with %d\n", GetLastError());
				GlobalFree((HGLOBAL)lpszBuffer);
				return;
			}
			//处理消息，显示
			lstrcat(lpszBuffer, (LPTSTR)achID);
			printf("Contents of the mailslot:%s\n", lpszBuffer);
			HeapFree(GetProcessHeap(), 0, lpszBuffer);
			//计算剩余的消息数
			fResult = GetMailslotInfo(hSlot, (LPDWORD)NULL, &cbMessage, &cMessage, (LPDWORD)NULL);
			if (!fResult)
			{
				printf("GetMailslotInfo failed (%d)\n", GetLastError());
				return;
			}
		}
	}
	return;
}