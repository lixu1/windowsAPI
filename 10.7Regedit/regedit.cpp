#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#pragma comment (lib,"Advapi32.lib")
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
void QueryKey(HKEY hKey)
{
	TCHAR achKey[MAX_KEY_LENGTH];
	DWORD cbName;
	TCHAR achClass[MAX_PATH] = TEXT("");
	DWORD cchClassName = MAX_PATH;
	DWORD cSubKeys = 0;
	DWORD cbMaxSubKey;
	DWORD cchMaxClass;
	DWORD cValues;
	DWORD cchMaxValue;
	DWORD cbMaxValueData;
	DWORD cbSecurityDescriptor;
	FILETIME ftLastWriteTime;
	DWORD i, retCode;
	TCHAR achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;
	retCode = RegQueryInfoKey(hKey, achClass, &cchClassName, NULL, &cSubKeys, &cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue, &cbMaxValueData, &cbSecurityDescriptor,
		&ftLastWriteTime);
	if (cSubKeys)
	{
		printf("\nNumber of subkeys:%d\n", cSubKeys);
		for (i = 0; i < cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				printf(TEXT("(%d) %s\n"), i + 1, achKey);
			}
		}
	}
	if (cValues)
	{
		printf("\nNumber of values:%d\n",cValues);
		for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			retCode = RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL);
			if (retCode == ERROR_SUCCESS)
			{
				printf(TEXT("(%d) %s\n"), i + 1, achValue);
			}
		}
	}
}

void AddKey(HKEY hKey)
{
	HKEY hSubKey;
	CONST BYTE dwKeyValue = 100;
	RegCreateKey(hKey, "MySoftware", &hSubKey);
	if (ERROR_SUCCESS != RegSetValueEx(hSubKey, "TEST", NULL, REG_DWORD, &dwKeyValue, sizeof(DWORD)))
	{
		printf("error\n");
	}
}
void main(void)
{
	HKEY hTestKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE"), 0, KEY_READ | KEY_WRITE, &hTestKey) == ERROR_SUCCESS)
	{
		AddKey(hTestKey);
		QueryKey(hTestKey);
	}
}