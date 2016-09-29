#include <Windows.h>
#include <stdio.h>
void ShowVersionInfo()
{
	OSVERSIONINFOEXA ovex;
	CHAR szVersionInfo[1024];
	*szVersionInfo = NULL;
	ovex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx(&ovex))
	{
		printf("error %d\n", GetLastError());
		return;
	}
	if (ovex.dwMajorVersion == 5)
	{
		if (ovex.dwMinorVersion == 0)
			lstrcat(szVersionInfo, "Windows 2000");
		if (ovex.dwMinorVersion == 1)
			lstrcat(szVersionInfo, "Windows XP");
		if (ovex.dwMinorVersion == 2)
			lstrcat(szVersionInfo, "Windows Server 2003");
	}
	else if (ovex.dwMajorVersion == 6)
		lstrcat(szVersionInfo, "Windows Vista");
	else
		lstrcat(szVersionInfo, "Windows NT 4.0 或者其他");
	lstrcat(szVersionInfo, ovex.szCSDVersion);
	switch (ovex.wProductType)
	{
	case VER_NT_DOMAIN_CONTROLLER:
		lstrcat(szVersionInfo, "\n域控制器");
		break;
	case VER_NT_SERVER:
		lstrcat(szVersionInfo, "\n服务器");
		break;
	case VER_NT_WORKSTATION:
		lstrcat(szVersionInfo, "\n独立工作站");
		break;
	}
	if (ovex.wSuiteMask&VER_SUITE_PERSONAL)
	{
		lstrcat(szVersionInfo, "\nWindows XP Home Edition");
	}
	if (ovex.wSuiteMask&VER_SUITE_SINGLEUSERTS)
	{
		lstrcat(szVersionInfo, "\n安装了终端服务，但只支持一个会话");
	}
	printf("%s\n", szVersionInfo);
}
void ShowSystemInfo()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	printf("内存分页大小：%0x%.8X，可用内存起始：0X%.8X，可用内存结束：0X%.8X,\n处理器个数：%d,处理器类型：", si.dwPageSize, si.lpMinimumApplicationAddress,
		si.lpMaximumApplicationAddress, si.dwNumberOfProcessors);
	switch (si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
		printf("386");
		break;
	case PROCESSOR_INTEL_486:
		printf("486");
		break;
	case PROCESSOR_INTEL_PENTIUM:
		printf("pentium");
		printf(",Cpu Model 0X%.2X, Stepping 0X%.2X", (BYTE)(si.wProcessorRevision >> 8), (BYTE)(si.wProcessorRevision));
		break;
	}
	printf("\n处理器架构：");
	switch (si.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		printf("intel CPU vendor is %d", si.wProcessorLevel);
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		printf("64 bits intel");
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		printf("64 bits AMD");
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		printf("UNKNOWN");
		break;
	}
	printf("\n");
}
//获取系统目录等信息
void GetFolders()
{
	TCHAR szSystemDirectory[MAX_PATH];
	TCHAR szWindowDirectory[MAX_PATH];
	GetSystemDirectory(szSystemDirectory,MAX_PATH);
	GetWindowsDirectory(szWindowDirectory, MAX_PATH);
	printf("系统目录:\t%s\nWindows 目录：\t%s\n", szSystemDirectory, szWindowDirectory);
}
//获取计算机名，用户名等信息
void GetNames()
{
	DWORD dwComputerNameLen = MAX_COMPUTERNAME_LENGTH + 1;
	DWORD dwUserNameLen = UNLEN + 1;
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	TCHAR szUserName[UNLEN + 1];
	COMPUTER_NAME_FORMAT cnf;
	if (!SetComputerName("My_Computer"))
	{
		printf("Set Error %d", GetLastError());
	}
	GetComputerName(szComputerName, &dwComputerNameLen);
	printf("计算机名：%s\n", szComputerName);
	dwComputerNameLen = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerNameEx(ComputerNameDnsHostname, szComputerName, &dwComputerNameLen);
	printf("ComputerNameDnsHostName:%s\n", szComputerName);
	dwComputerNameLen = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerNameEx(ComputerNamePhysicalNetBIOS, szComputerName, &dwComputerNameLen);
	printf("ComputerNamePhysicalNetBIOS:%s\n", szComputerName);
	printf("用户名：%s\n", szUserName);
}
//获取鼠标速度
void MouseSpeed()
{
	BOOL fResult;
	int aMouseInfo[3];
	fResult = SystemParametersInfo(SPI_GETMOUSE, 0, &aMouseInfo, 0);
	if (fResult)
	{
		aMouseInfo[2] = 2 * aMouseInfo[2];
		SystemParametersInfo)(SPI_SETMOUSE, 0, aMouseInfo, SPIF_SENDCHANGE);
	}
}
int main()
{
	ShowVersionInfo();
}