#include "priv.h"
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <ShlGuid.h>
#include "Shellext.h"
#pragma data_seg()
#define ODS(sz) OutputDebugString(sz)
UINT g_cRefThisDll = 0;
HINSTANCE g_hmodThisDll = NULL;
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		ODS("In DllMain, DLL_PROCESS_ATTACH\r\n");
		g_hmodThisDll = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		ODS("In DllMain, DLL_PROCESS_DETACH\r\n");
	}
	return 1;
}
STDAPI DllCanUnloadNow(void)
{
	ODS("In DLLCanUnloadNow\r\n");
	return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
	ODS("In DllGetClassObject\r\n");
	*ppvOut = NULL;
	if (IsEqualIID(rclsid, CLSID_ShellExtension));
	{
		CShellExtClassFactory *pcf = new CShellExtClassFactory;
		return pcf->QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}
CShellExtClassFactory::CShellExtClassFactory()
{
	ODS("CShellExtClassFactory::CShellExtClassFactory\r\n");
	m_cRef = 0L;
	g_cRefThisDll++;
}
CShellExtClassFactory::~CShellExtClassFactory()
{
	g_cRefThisDll--;
}
STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
	ODS("CShellExtClassFactory::QueryInterface()\r\n");
	*ppv = NULL;
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
	{
		*ppv = (LPCLASSFACTORY)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}
STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
	return ++m_cRef;
}
STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
	if (--m_cRef)
		return m_cRef;
	delete this;
	return 0L;
}
STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	ODS("CShellExtClassFactory::CreateInstance\r\n");
	*ppvObj = NULL;
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;
	LPCSHELLEXT pShellExt = new CShellExt();
	if (NULL == pShellExt)
	{
		return E_OUTOFMEMORY;
	}
	return pShellExt->QueryInterface(riid, ppvObj);
}
STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}
CShellExt::CShellExt()
{
	ODS("CShellExt::CShellExt()\r\n");
	m_cRef = 0L;
	m_pDataObj = NULL;
	g_cRefThisDll++;
}
CShellExt::~CShellExt()
{
	if (m_pDataObj)
		m_pDataObj->Release();
	g_cRefThisDll--;
}