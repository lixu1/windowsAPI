#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include <commctrl.h>
HWND hwndTreeView, hwndListView, hwndEdit, hwndMain;
HINSTANCE hinst;
LPSTR lpszLatin = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed to eiusmod tempor ";
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			ExitProcess(0);
		}
		break;
	}
	return FALSE;
}
DWORD SetWindows(LPRECT lpRect)
{
	SetWindowPos(hwndTreeView, HWND_TOP, lpRect->left, lpRect->top, lpRect->right*0.3, lpRect->bottom, SWP_SHOWWINDOW);
	SetWindowPos(hwndListView, HWND_TOP, lpRect->right*0.3, lpRect->bottom*0.7, lpRect->right*0.7, lpRect->bottom*0.3, SWP_SHOWWINDOW);
	SetWindowPos(hwndEdit, HWND_TOP, lpRect->right*0.3, lpRect->top, lpRect->right*0.7, lpRect->bottom*0.7, SWP_SHOWWINDOW);
	return 0;
}
VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt)
{
	HMENU hmenu;
	HMENU hmenuTrackPopup;
	if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(IDR_MENU_POPUP))) == NULL)
		return;
	hmenuTrackPopup = GetSubMenu(hmenu, 0);
	TrackPopupMenu(hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y,0, hwnd, NULL);
	DestroyMenu(hmenu);
}
BOOL OnRclickTree(NMHDR * pNMHDR)
{
	POINT point;
	TVHITTESTINFO thti;
	HTREEITEM htItem;
	GetCursorPos(&point);
	ScreenToClient(hwndTreeView, &point);
	thti.pt = point;
	thti.flags = TVHT_TORIGHT;
	htItem = TreeView_HitTest(hwndTreeView, &thti);
	if (htItem != NULL)
	{
		ClientToScreen(hwndTreeView, &point);
		TreeView_SelectItem(hwndTreeView, htItem);
		DisplayContextMenu(hwndMain, point);
		return TRUE;
	}
	return FALSE;
}
BOOL OnChildWindowsNotify(PVOID pParam)
{
	LPNMHDR phdr = (LPNMHDR)pParam;
	if (phdr->hwndFrom != hwndTreeView)
	{
		return FALSE;
	}
	switch (((LPNMHDR)pParam)->code)
	{
	case NM_RCLICK:
		OnRclickTree((LPNMHDR)pParam);
		break;
	default:
		break;
	}
	return TRUE;
}
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		break;
	case WM_SIZE:
		OnWindowResize();
		break;
	case WM_NOTIFY:
		OnChildWindowsNotify((PVOID)lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_HELP_ABOUT:
		{
			DialogBox(hinst, (LPCTSTR)IDD_DIALOG_ABOUT, hwnd, (DLGPROC)About);
			return 0;
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
BOOL InitApplication(HINSTANCE hinstance)//注册主窗口类
{
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = MainWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hinstance;
	wcx.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_MAIN));
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);
	wcx.lpszClassName = "MainWClass";
	wcx.hIconSm = (HICON)LoadImage(hinstance, MAKEINTRESOURCE(IDI_ICON_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	return RegisterClassEx(&wcx);
}
BOOL InitTreeViewImageLists(HWND hwndTV)
{
	//create imagelists
	//set ImageLists
	return TRUE;
}
HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, HTREEITEM hParent, BOOL bFolder)
{
	TVITEM tvi;
	TVITEM tParent;
	TVINSERTSTRUCT tvins;
	HTREEITEM hme;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
	if (bFolder)
	{
		tvi.iImage = 1;
		tvi.iSelectedImage = 2;
		tvi.cChildren = I_CHILDRENCALLBACK;
	}
	else
	{
		tvi.iImage = 3;
		tvi.iSelectedImage = 3;
		tvi.cChildren = 0;
	}
	tvins.item = tvi;
	tvins.hInsertAfter = TVI_SORT;
	if (hParent == NULL)
	{
		tvins.hParent = TVI_ROOT;
	}
	else
	{
		tvins.hParent = hParent;
	}
	hme = TreeView_InsertItem(hwndTV, &tvins);
	return hme;
}
BOOL InitTreeViewItems(HWND hwndTV)
{
	HTREEITEM hRoot = AddItemToTree(hwndTV, "type1", NULL, TRUE);
	HTREEITEM hP = AddItemToTree(hwndTV, "name1", hRoot, FALSE);
	hP = AddItemToTree(hwndTV, "name2", hRoot, TRUE);
	AddItemToTree(hwndTV, "sub1", hP, FALSE);
	AddItemToTree(hwndTV, "sub2", hP, FALSE);
	hRoot = AddItemToTree(hwndTV, "type2", NULL, TRUE);
	hP = AddItemToTree(hwndTV, "name3", hRoot, TRUE);
	hP = AddItemToTree(hwndTV, "sub3", hP, FALSE);
	hRoot = AddItemToTree(hwndTV, "type3", NULL, TRUE);
	return TRUE;
}
HWND CreateTreeView(HWND hwndParent, LPSTR szWindowName)
{
	HWND hwndTV;
	hwndTV = CreateWindowEx(0, WC_TREEVIEW, szWindowName, WS_VISIBLE | WS_CHILD | WS_SIZEBOX | WS_TILED | TVS_HASBUTTONS | TVS_LINESATROOT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, (HMENU)NULL, hinst, NULL);
	if (!InitTreeViewImageLists(hwndTV) || !InitTreeViewItems(hwndTV))
	{
		DestoryWindow(hwndTV);
		return NULL;
	}
	return hwndTV;
}
BOOL GetImageList(HIMAGELIST *pHiml)
{
	return TRUE;
}
BOOL InitListViewImageLists(HWND hWndListView)
{
	HIMAGELIST himl;
	HBITMAP hmap;
	GetImageList(&himl);
	ListView_SetImageList(hWndListView, himl, LVSIL_SMALL);
	return TRUE;
}
//设置ListView的分栏
BOOL InitListViewColums(HWND hWndListView)
{
	char szText[256];
	LVCOLUMN lvc;
	DWORD i;
	LPSTR ColNames[] = { "name", "type", "size", "time" };
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.pszText = szText;
	lvc.cx = 100;
	lvc.iImage = 1;
	lvc.fmt = LVCFMT_LEFT;
	for (i = 0; i < 3; i++)
	{
		lvc.pszText = ColNames[i];
		lvc.iSubItem = i;
		if (ListView_InsertColumn(hWndListView, i, &lvc) == -1)
		{
			return FALSE;
		}
	}
	lvc.cx = 200;
	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = 3;
	lvc.pszText = ColNames[3];
	if (ListView_InsertColumn(hWndListView, 3, &lvc) == -1)
	{
		return FALSE;
	}
	return TRUE;
}
//为ListView增加项
BOOL AddListViewItems(HWND hwndListView)
{
	LVITEM lvI;
	DWORD index;
	ZeroMemory(&lvI, sizeof(lvI));
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvI.state = 0;
	lvI.stateMask = 0;
	for (index = 0; index < 3; index++)
	{
		lvI.iItem = index;
		lvI.iImage = 0;
		lvI.iSubItem = 0;
		lvI.pszText = "name1";
		lvI.cchTextMax = lstrlen(lvI.pszText) + 1;
		if (ListView_InsertItem(hwndListView, &lvI) == -1)
			return FALSE;
		ListView_SetItemText(hwndListView, index, 2, "size a");
	}
	return TRUE;
}
HWND CreateListView(HWND hwndParent, LPSTR szWindowName)
{
	HWND hWndListView;
	hWndListView = CreateWindow(WC_LISTVIEW, szWindowName, WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, NULL, hinst, NULL);
	if (hWndListView == NULL)
	{
		return NULL;
	}
	if (InitListViewImageLists(hWndListView) && InitListViewColums(hWndListView) && AddListViewItems(hWndListView))
	{
		return hWndListView;
	}
	DestroyWindow(hWndListView);
	return FALSE;
}
HWND CreateEdit(HWND hwndP, LPSTR szTitle)
{
	WNDCLASSEX wcx;
	HWND hwnd;
	hwnd = CreateWindow("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndP, (HMENU)NULL, hinst, (LPVOID)NULL);
	if (!hwnd)
		return NULL;
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	return hwnd;
}
DWORD ShowTextOnEdit(LPSTR szTextToShow)
{
	SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)szTextToShow);
	return 0;
}
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	HWND hwnd;
	RECT rect;
	hinst = hinstance;
	hwnd = CreateWindow("MainWClass", "Let's Smail", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUPWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, (HWND)NULL, (HMENU)LoadMenu(hinst, MAKEINTRESOURCE(IDR_MENU_MAIN)), hinstance, (LPVOID)NULL);
	if (!hwnd)
		return FALSE;
	hwndMain = hwnd;
	InitCommonControls();
	hwndTreeView = CreateTreeView(hwndMain, "files");
	hwndListView = CreateListView(hwndMain, "processing");
	hwndEdit = CreateEdit(hwndMain, "texta");
	GetClientRect(hwndMain, &rect);
	SetWindows(&rect);
	ShowTextOnEdit(lpszLatin);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL fGotMessage;
	//register window
	if (!InitApplication(hinstance))
	{
		return FALSE;
	}
	//create window
	if (!InitInstance(hinstance, nCmdShow))
	{

	}
	while ((fGotMessage = GetMessage(&msg, (HWND)NULL, 0, 0) != 0 && fGotMessage != -1))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}
