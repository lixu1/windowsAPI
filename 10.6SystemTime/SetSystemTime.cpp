#include <Windows.h>
#include <stdio.h>
int main()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	printf("Now:%d-%d-%d,%d:%d:%d", st.wYear, st.wMonth, st.wSecond,st.wHour,st.wMinute,st.wSecond);
	st.wHour--;
	SetLocalTime(&st);
}