#include <Windows.h>
#include <stdio.h>
#include <ShlObj.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "gdi32.lib")
DWORD Browse(HWND hwnd);

//获取并显示特殊目录
VOID GetSpecialFolder()
{
	CHAR szMyDocument[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szMyDocument, CSIDL_PERSONAL, FALSE);
	CHAR szDesktop[MAX_PATH];
	LPITEMIDLIST pid1 = NULL;
	LPMALLOC pMalloc = NULL;
	SHGetMalloc(&pMalloc);
	SHGetFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, &pid1);
	SHGetPathFromIDList(pid1, szDesktop);
	pMalloc->Free(pid1);
	pMalloc->Release();
	printf("My Document:\t %s\n", szMyDocument);
	printf("Desktop:\t %s\n", szDesktop);
}
//遍历并显示回收站中的文件
DWORD ListFileInRecycleBin()
{
	CHAR pszPath[MAX_PATH];
	IShellFolder *pisf = NULL;
	IShellFolder *pisfRecBin = NULL;
	SHGetDesktopFolder(&pisfRecBin);
	IEnumIDList *peid1 = NULL;
	LPITEMIDLIST pidlBin = NULL;
	LPITEMIDLIST idlCurrent = NULL;
	LPMALLOC pMalloc = NULL;
	SHGetMalloc(&pMalloc);
	SHGetFolderLocation(NULL, CSIDL_BITBUCKET, NULL, 0, &pidlBin);
	pisfRecBin->BindToObject(pidlBin, NULL, IID_IShellFolder, (void **)&pisf);
	pisf->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &peid1);
	STRRET strret;
	ULONG uFetched;
	HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
	printf("\nFiles In Recycle Bin:\n");
	while (1)
	{
		if (peid1->Next(1, &idlCurrent, &uFetched) == S_FALSE)
			break;
		SHGetPathFromIDList(idlCurrent, pszPath);
		pisf->GetDisplayNameOf(idlCurrent, SHGDN_NORMAL, &strret);
		WriteConsoleW(hOutPut, L"\t", 1, NULL, NULL);
		WriteConsoleW(hOutPut, strret.pOleStr, lstrlenW(strret.pOleStr), NULL, NULL);
		WriteConsoleW(hOutPut, L"\n", 1, NULL, NULL);
	}
	pMalloc->Free(pidlBin);
	pMalloc->Free(strret.pOleStr);
	pMalloc->Release();
	peid1->Release();
	pisf->Release();
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Browse(NULL);
}
DWORD Browse(HWND hwnd)
{
	CHAR szRoot[MAX_PATH];
	CHAR szChoose[MAX_PATH];
	CHAR szDisplayName[MAX_PATH];
	LPITEMIDLIST pid1Root = NULL;
	LPITEMIDLIST pid1Selected = NULL;
	BROWSEINFO bi = { 0 };
	LPMALLOC pMalloc = NULL;
	SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, 0, &pid1Root);
	SHGetPathFromIDList(pid1Root, szRoot);
	bi.hwndOwner = hwnd;
	bi.pidlRoot = pid1Root;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = "Choose a target";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	pid1Selected = SHBrowseForFolder(&bi);
	MessageBox(NULL, szDisplayName, "Display Name:", MB_OK);
	SHGetPathFromIDList(pid1Selected, szChoose);
	MessageBox(NULL, szChoose, "Choose:", MB_OK);
	ILFree(pid1Root);
	return 0;
}