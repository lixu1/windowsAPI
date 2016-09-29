#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "user32.lib")
void ScrollScreenBuffer(HANDLE, INT)
{

}
VOID HandleInput(LPSTR szInput);
HANDLE hStdout, hStdin;
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
VOID MyErrorExit(LPSTR error)
{
	MessageBox(NULL, error, "Error", MB_OK);
}
VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
	CHAR szMsg[256];
	wsprintf(szMsg, "KEY_EVENT_RECORD\n CHAR=%c", ker.uChar);
	MessageBox(NULL, szMsg, "EVENT", MB_OK);
}
VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
	CHAR szMsg[256];
	wsprintf(szMsg, "MOUSE_EVENT_RECORD\n button state:%d\nMouse position X=%u,Y=%u", mer.dwButtonState, mer.dwMousePosition.X, mer.dwMousePosition.Y);
	MessageBox(NULL, szMsg, "EVENT", MB_OK);
	if (IDOK == MessageBox(NULL, "Exit?", "EVENT", MB_OK))
	{
		ExitProcess(0);
	}
}
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
	CHAR szMsg[256];
	wsprintf(szMsg, "WINDOW_BUFFER_RECORD\n X=%u,Y=%u", wbsr.dwSize.X, wbsr.dwSize.Y);
	MessageBox(NULL, szMsg, "EVENT", MB_OK);
}
DWORD UseEvent(VOID)
{
	CHAR chBuffer[256];
	DWORD cRead;
	HANDLE hStdin;
	DWORD cNumRead, fdwMode, fdwSaveOldMode, i;
	INPUT_RECORD irInBuf[128];
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE)
		MyErrorExit("GetStdHandle");
	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
		MyErrorExit("GetConsoleMode");
	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(hStdin, fdwMode))
		MyErrorExit("SetConsoleMode");
	while (1)
	{
		if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead))
			MyErrorExit("ReadConsoleInput");
		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
				KeyEventProc(irInBuf[i].Event.KeyEvent);
				break;
			case MOUSE_EVENT:
				MouseEventProc(irInBuf[i].Event.MouseEvent);
				break;
			case FOCUS_EVENT:
			case MENU_EVENT:
				break;
			default:
				MyErrorExit("unknown event type");
				break;
			}
		}
	}
	return 0;
}
void NewLine(void)
{
	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		MyErrorExit("GetConsoleScreenBufferInfo");
		return;
	}
	csbiInfo.dwCursorPosition.X = 0;
	if ((csbiInfo.dwSize.Y - 1) == csbiInfo.dwCursorPosition.Y)
	{
		ScrollScreenBuffer(hStdout, 1);
	}
	else
		csbiInfo.dwCursorPosition.Y += 1;
	if (!SetConsoleCursorPosition(hStdout, csbiInfo.dwCursorPosition))
	{
		MyErrorExit("SetConsoleCursorPosition");
		return;
	}
}
VOID ChangeMode(HANDLE hStdin, HANDLE hStdout)
{
	LPSTR lpszPrompt = "Mode changed Type any key,or q to quit";
	CHAR chBuffer[256];
	DWORD fdwMode, fdwOldMode;
	DWORD cRead, cWritten;
	if (!GetConsoleMode(hStdin, &fdwOldMode))
	{
		MessageBox(NULL, "GetConsoleMode", "Console Error", MB_OK);
		return;
	}
	fdwMode = fdwOldMode&~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	if (!SetConsoleMode(hStdin, fdwMode))
	{
		MyErrorExit("SetConsoleMode");
		return;
	}
	NewLine();
	while (1)
	{
		if (!WriteFile(hStdout, lpszPrompt, lstrlen(lpszPrompt), &cWritten, NULL))
		{
			MyErrorExit("WriteFile");
			return;
		}
		if (!ReadFile(hStdin, chBuffer, 1, &cRead, NULL))
			break;
		if (chBuffer[0] == '\r')
			NewLine();
		else if (!WriteFile(hStdout, chBuffer, cRead, &cWritten, NULL))break;
		else
			NewLine();
		if (chBuffer[0] == 'q')break;
	}
}
VOID UseBlockIO()
{
	HANDLE hStdout, hNewScreenBuffer;
	SMALL_RECT srctReadRect;
	SMALL_RECT srctWriteRect;
	CHAR_INFO chiBuffer[160]; 
	COORD coordBufSize;
	COORD coordBufCoord;
	BOOL fSuccess;
	hStdout = GetStdHandle(STD_ERROR_HANDLE);
	hNewScreenBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (hStdout == INVALID_HANDLE_VALUE || hNewScreenBuffer == INVALID_HANDLE_VALUE)
	{
		printf("CreateConsoleScreenBuffer (%d)\n", GetLastError());
		return;
	}
	if (!SetConsoleActiveScreenBuffer(hNewScreenBuffer))
	{
		printf("SetConsoleActiveScreenBuffer (%d)\n", GetLastError());
		return;
	}
	//source cell 
	srctReadRect.Top = 0;
	srctReadRect.Left = 0;
	srctReadRect.Bottom = 0;
	srctReadRect.Right = 79;
	coordBufSize.Y = 2;
	coordBufSize.X = 80;
	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;
	fSuccess = ReadConsoleOutput(hStdout, chiBuffer, coordBufSize, coordBufCoord, &srctReadRect);
	if (!fSuccess)
	{
		printf("ReadConsoleOutput (%d) \n", GetLastError());
		return;
	}
	srctWriteRect.Top = 10;
	srctWriteRect.Left = 0;
	srctWriteRect.Bottom = 11;
	srctWriteRect.Right = 79;
	fSuccess = WriteConsoleOutput(hNewScreenBuffer, chiBuffer, coordBufSize, coordBufCoord, &srctWriteRect);
	if (!fSuccess)
	{
		printf("WriteConsoleOutput (%d)\n", GetLastError());
		return;
	}
	Sleep(10000);
	if (!SetConsoleActiveScreenBuffer(hStdout))
	{
		printf("SetConsoleActiveScreenBuffer (%d)\n", GetLastError());
		return;
	}
}
VOID cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return;
	dwConSize = csbi.dwSize.X*csbi.dwSize.Y;
	if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten))
		return;
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return;
	if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten))
		return;
	SetConsoleCursorPosition(hConsole, coordScreen);
}

VOID ChangeConsoleTitle(LPSTR szNewTitle)
{
	if (!SetConsoleTitle(szNewTitle))
		MyErrorExit("SetConsoleTitle failed\n");
}
VOID ChangeTextColor(HANDLE hStdout, WORD wColor)
{
	if (!SetConsoleTextAttribute(hStdout, wColor))
		MyErrorExit("SetConsoleTextAttribute");
}
VOID ChangeBackgroundColor(HANDLE hConsole)
{
	COORD coord;
	WORD wColor;
	DWORD cWritten;
	BOOL fSuccess;
	coord.X = 0;
	coord.Y = 0;
	wColor = BACKGROUND_BLUE | BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	fSuccess = FillConsoleOutputAttribute(hConsole, wColor, 80 * 50, coord, &cWritten);
	if (fSuccess)
		MyErrorExit("FillConsoleOutputAttribute");
}
VOID ChangeUpCaseTextBackgroundColor(HANDLE hStdin, HANDLE hStdout)
{
	DWORD dwLen, i, dwRead, dwWritten;
	WORD wColors[3];
	BOOL fSuccess;
	COORD coord;
	HANDLE hScreenHandle;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	PCHAR lpCharacter = (PCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 2048);
	coord.X = 0;
	coord.Y = 0;
	if (!ReadConsoleOutputCharacter(hStdout, lpCharacter, 2047, coord, &dwRead))
		MyErrorExit("ReadConsoleOutputCharacter");
	GetConsoleScreenBufferInfo(hStdout, &csbi);
	dwLen = lstrlen(lpCharacter);
	wColors[0] = BACKGROUND_RED;
	wColors[1] = BACKGROUND_RED | BACKGROUND_GREEN;
	wColors[2] = BACKGROUND_GREEN;
	for (i = 0; i < dwLen; i++)
	{
		if (lpCharacter[i] >= 'A'&&lpCharacter[i] <= 'Z')
		{
			coord.Y = i / csbi.dwSize.X;
			coord.X = i%csbi.dwSize.Y;
			fSuccess = WriteConsoleOutputAttribute(hStdout, wColors, 3, coord, &dwWritten);
			if (!fSuccess)
				MyErrorExit("WriteConsoleOutputAttribute");
		}
	}
}
VOID GetConsoleInfo(HANDLE hOutput)
{
	CONSOLE_FONT_INFO cfi;
	DWORD dwProcessList[32];
	DWORD dwAttachedProcess;
	TCHAR szOutputBuffer[1024];
	HWND hConsoleWindows;
	DWORD dwWritten;
	DWORD i;
	CHAR szConsoleTitle[MAX_PATH];
	dwAttachedProcess = GetConsoleProcessList(dwProcessList, 32);
	if (dwAttachedProcess == 0)
	{
		MyErrorExit("GetConsoleProcessList failed\n");
	}
	if (!GetConsoleTitle(szConsoleTitle, MAX_PATH))
	{
		MyErrorExit("GetConsoleTitle");
	}
	hConsoleWindows = GetConsoleWindow();
	GetCurrentConsoleFont(hOutput, FALSE, &cfi);
	wsprintf(szOutputBuffer, "Now %u attached Processes: ", dwAttachedProcess);
	for (i = 0; i < dwAttachedProcess; i++)
	{
		wsprintf(szOutputBuffer + lstrlen(szOutputBuffer), "%u, ", dwProcessList[i]);
	}
	wsprintf(szOutputBuffer + lstrlen(szOutputBuffer), "\nConsoleTitle is %s, \nWindow Handle is 0x%.8X\n"
		"Font is %u, Font Size X=%u,Y=%u\n", szConsoleTitle, hConsoleWindows, cfi.nFont, cfi.dwFontSize.X, cfi.dwFontSize.Y);
	if (!WriteConsole(hOutput, szOutputBuffer, lstrlen(szOutputBuffer), &dwWritten, NULL))
	{
		MyErrorExit("WriteConsole");
	}
}
void main(void)
{
	LPSTR lpszPrompt = "Type a line and press Enter,255 max\nq to quit"
		"\n i to Show console info\n c to Change background color\n s to Change text color"
		"\n t to Find upper case character\n b to Call UseBlockIO \n u to Change conslos title"
		"\n m to Show console event\n r to Change mode \n p to Clear up Console\n";
	CHAR chBuffer[256];
	DWORD cRead, cWritten, fdwMode, fdwOldMode;
	WORD wOldColorAttrs;
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE || hStdout == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, "GetStdHandle", "Console Error", MB_OK);
		return;
	}
	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		MessageBox(NULL, "GetConsoleScreenBufferInfo", "Console Error", MB_OK);
		return;
	}
	wOldColorAttrs = csbiInfo.wAttributes;
	ChangeTextColor(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
	if (WriteConsole(hStdout, lpszPrompt, lstrlen(lpszPrompt), &cWritten, NULL))
	{
		MessageBox(NULL, "WriteFile", "Console Error", MB_OK);
		return;
	}
	while (1)
	{
		ZeroMemory(chBuffer, 256);
		if (!ReadConsole(hStdin, chBuffer, 255, &cRead, NULL))break;
		HandleInput(chBuffer);
	}
}
VOID HandleInput(LPSTR szInput)
{
	switch (szInput[0])
	{
	case 'q':
		ExitProcess(0);
	case 'i':
		GetConsoleInfo(hStdout);
		break;
	case 'c':
		ChangeBackgroundColor(hStdout);// , FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case 'b':
		UseBlockIO();
		break;
	case 'u':
		ChangeConsoleTitle(&szInput[2]);
		break;
	case 'm':
		UseEvent();
		break;

	}
}