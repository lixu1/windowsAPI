#include <Windows.h>
SERVICE_STATUS SplSrvServiceStatus;
SERVICE_STATUS_HANDLE SplSrvServiceStatusHandle;
VOID SvcDebugOut(LPSTR String, DWORD Status);
VOID WINAPI SplSrvServiceCtrlHandler(DWORD opcode);
VOID WINAPI SplSrvServiceStart(DWORD argc, LPTSTR *argv);
DWORD SplSrvServiceInitialization(DWORD argc, LPTSTR *argv, DWORD *specificError);
VOID WINAPI SplSrvServiceStart(DWORD argc, LPTSTR *argv)
{
	DWORD status;
	DWORD specificError;
	SplSrvServiceStatus.dwServiceType = SERVICE_WIN32;
	SplSrvServiceStatus.dwCurrentState = SERVICE_START_PENDING;//service is running
	SplSrvServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	SplSrvServiceStatus.dwWin32ExitCode = 0;
	SplSrvServiceStatus.dwServiceSpecificExitCode = 0;
	SplSrvServiceStatus.dwCheckPoint = 0;
	SplSrvServiceStatus.dwWaitHint = 0;
	SplSrvServiceStatusHandle = RegisterServiceCtrlHandler("Sample_Srv", SplSrvServiceCtrlHandler);
	if (SplSrvServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		SvcDebugOut("[SPLSRV_SERVICE] RegisterServiceCtrlHandle failed %d\n", GetLastError());
		return;
	}
	status = SplSrvServiceInitialization(argc, argv, &specificError);
	if (status != NO_ERROR)
	{
		SplSrvServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SplSrvServiceStatus.dwCheckPoint = 0;
		SplSrvServiceStatus.dwWaitHint = 0;
		SplSrvServiceStatus.dwWin32ExitCode = status;
		SplSrvServiceStatus.dwServiceSpecificExitCode = specificError;
		SetServiceStatus(SplSrvServiceStatusHandle, &SplSrvServiceStatus);
		return;
	}
	SplSrvServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SplSrvServiceStatus.dwCheckPoint = 0;
	SplSrvServiceStatus.dwWaitHint = 0;
	if (!SetServiceStatus(SplSrvServiceStatusHandle, &SplSrvServiceStatus))
	{
		status = GetLastError();
		SvcDebugOut("[SPLSRV_SERVICE] SetServiceStatus error %ld\n", status);
	}
	SvcDebugOut("[SPLSRV_SERVICE] Returning the Main Thread \n", 0);
	return;
}

DWORD SplSrvServiceInitialization(DWORD argc, LPTSTR *argv, DWORD *specificError)
{
	return 0;
}
VOID WINAPI SplSrvServiceCtrlHandle(DWORD Opcode)
{
	DWORD status;
	switch (Opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		SplSrvServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		SplSrvServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_STOP:
		SplSrvServiceStatus.dwWin32ExitCode = 0;
		SplSrvServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SplSrvServiceStatus.dwCheckPoint = 0;
		SplSrvServiceStatus.dwWaitHint = 0;
		if (!SetServiceStatus(SplSrvServiceStatusHandle, &SplSrvServiceStatus))
		{
			status = GetLastError();
			SvcDebugOut("[SPLSRV_SERVICE] SetServiceStatus error %ld\n", status);
		}
		SvcDebugOut("[SPLSRV_SERVICE] Leaving SplSrvService \n", 0);
		return;
	case SERVICE_CONTROL_INTERROGATE:
		//MesageBeep(MB_OK);
		break;
	default:
		SvcDebugOut(" [SPLSRV_SERVICE] Unrecognized opcode %ld\n", Opcode);
	}
	if (!SetServiceStatus(SplSrvServiceStatusHandle, &SplSrvServiceStatus))
	{
		status = GetLastError();
		SvcDebugOut("[SPLSRV_SERVICE] SetServiceStatus error %ld\n", status);
	}
	return;
}
void main()
{
	SERVICE_TABLE_ENTRY DispatchTable[]=
	{
		{ "Sample_Srv", (LPSERVICE_MAIN_FUNCTION)SplSrvServiceStart },
		{NULL,NULL}
	};
	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		SvcDebugOut("[SPLSRV_SERVICE] StartServiceCtrlDispatcher (%d)\n", GetLastError());
	}
}
VOID SvcDebugOut(LPSTR String, DWORD Status)
{
	CHAR Buffer[1024];
	if (strlen(String) < 1000)
	{
		wsprintf(Buffer, String, Status);
		OutputDebugString(Buffer);
	}
}