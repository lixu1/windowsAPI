/* ͷ�ļ���*/
#include <windows.h>
#include <Tlhelp32.h>
#include <stdio.h>
/*************************************
* BOOL EnablePrivilege (PCSTR name)
* ����	������Ȩ��
*
* ����	PCSTR name	�����Ȩ��
* �����Ƿ�ɹ�
**************************************/
DWORD EnablePrivilege(PCSTR name)
{
	HANDLE hToken;
	BOOL rv;
	//���ýṹ
	TOKEN_PRIVILEGES priv = { 1, { 0, 0, SE_PRIVILEGE_ENABLED } };
	// ����Ȩ��ֵ
	LookupPrivilegeValue(
		0,
		name,
		&priv.Privileges[0].Luid
		);
	// �򿪱�����Token
	OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES,
		&hToken
		);
	// ��Ȩ
	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&priv,
		sizeof priv,
		0,
		0
		);
	// ����ֵ��������Ϣ����������ɹ�����ӦΪERROR_SUCCESS��ΪO
	rv = GetLastError();
	// �ر�Token
	CloseHandle(hToken);
	return rv;
}

/*************************************
* BOOL LoadRometeDll(DWORD dwProcessId, LPTSTR lpszLibName)
* ����	ͨ������Զ���̸߳��������̼���Dll
*
* ����	DWORD dwProcessId	Ŀ�����PID
*		LPTSTR lpszLibName	Dll��·��
* �����Ƿ�ɹ�
**************************************/
BOOL LoadRometeDll(DWORD dwProcessId, LPTSTR lpszLibName)
{
	BOOL   bResult = FALSE;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	PSTR   pszLibFileRemote = NULL;
	DWORD  cch;
	PTHREAD_START_ROUTINE pfnThreadRtn;

	__try
	{
		// �����Ҫע�����Ľ��̵ľ��.
		hProcess = OpenProcess(
			PROCESS_ALL_ACCESS,
			FALSE,
			dwProcessId
			);

		if (hProcess == NULL)
			__leave;

		// ����DLL·������Ҫ���ֽ���.
		cch = 1 + lstrlen(lpszLibName);

		// ��Զ���߳���Ϊ·��������ռ�.
		pszLibFileRemote = (PSTR)VirtualAllocEx(
			hProcess,
			NULL,
			cch,
			MEM_COMMIT,
			PAGE_READWRITE
			);

		if (pszLibFileRemote == NULL)
			__leave;

		// ��DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�.
		if (!WriteProcessMemory(
			hProcess,
			(PVOID)pszLibFileRemote,
			(PVOID)lpszLibName,
			cch,
			NULL))
			__leave;

		// ���LoadLibraryA��Kernel32.dll�е�������ַ. 
		pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(
			GetModuleHandle(TEXT("Kernel32")), TEXT("LoadLibraryA"));

		if (pfnThreadRtn == NULL)
			__leave;

		// ����Զ���̣߳���ͨ��Զ���̵߳����û���DLL�ļ�. 
		hThread = CreateRemoteThread(
			hProcess,
			NULL,
			0,
			pfnThreadRtn,
			(PVOID)pszLibFileRemote,
			0,
			NULL
			);
		if (hThread == NULL)
			__leave;

		// �ȴ�Զ���߳���ֹ.
		WaitForSingleObject(hThread, INFINITE);
		bResult = TRUE;
	}
	__finally
	{
		// �رվ��. 
		if (pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, (PVOID)pszLibFileRemote, 0, MEM_RELEASE);
		if (hThread != NULL)
			CloseHandle(hThread);
		if (hProcess != NULL)
			CloseHandle(hProcess);
	}
	return bResult;
}
/*************************************
* BOOL GetProcessIdByName(LPSTR szProcessName, LPDWORD lpPID)
* ����	ͨ����������ȡ����PID
*
* ����	LPSTR szProcessName	������
*		LPDWORD lpPID		ָ�򱣴�PID�ı���
* �����Ƿ�ɹ�
**************************************/
BOOL GetProcessIdByName(LPSTR szProcessName, LPDWORD lpPID)
{
	// ��������ʼ��
	STARTUPINFO st;
	PROCESS_INFORMATION pi;
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;
	ZeroMemory(&st, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	st.cb = sizeof(STARTUPINFO);
	ZeroMemory(&ps, sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);
	// ��������
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (!Process32First(hSnapshot, &ps))
	{
		return FALSE;
	}
	do
	{
		// �ȽϽ�����
		if (lstrcmpi(ps.szExeFile, "explorer.exe") == 0)
		{
			// �ҵ���
			*lpPID = ps.th32ProcessID;
			CloseHandle(hSnapshot);
			return TRUE;
		}
	} while (Process32Next(hSnapshot, &ps));
	// û���ҵ�
	CloseHandle(hSnapshot);
	return FALSE;
}
/*************************************
* int WinMain(
*			HINSTANCE hInstance,
*			HINSTANCE hPrevInstance,
*			LPSTR lpCmdLine,
*			int nCmdShow
*			)
**************************************/
int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
	)
{
	DWORD dwPID;
	// ��Ȩ����ȡSE_DEBUG_NAMEȨ�ޣ�
	// �������������̵��ڴ�ռ���д�롢�����߳�
	if (0 != EnablePrivilege(SE_DEBUG_NAME))
	{
		printf("error");
		return 0;
	}
	// ��ȡĿ¼���̵�PID
	if (!GetProcessIdByName("explorer.exe", &dwPID)){
		printf("error");
		return 0;
	}
	// ͨ������Զ���̼߳���DLL
	// ��msg.dll������ϵͳĿ¼��
	if (!LoadRometeDll(dwPID, "msg.dll")){
		printf("error");
		return 0;
	}
	return 1;
}