#include "stdafx.h"
#include "wmihard.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <Shlwapi.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "shlwapi.lib")

void get_wmi_init()
{
	static bool isinit = false;
	if (isinit)
	{
		return;
	}
	isinit = true;
	HRESULT hres =  CoInitializeSecurity(
						NULL,
						-1,                          // COM authentication
						NULL,                        // Authentication services
						NULL,                        // Reserved
						RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
						RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
						NULL,                        // Authentication info
						EOAC_NONE,                   // Additional capabilities
						NULL                         // Reserved
					);

	if (FAILED(hres))
	{
		_com_issue_error(hres);
	}
}

std::list<std::string> get_wmi_serial(const std::string& key)
{
	HRESULT hres;

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------
	get_wmi_init();

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator* pLoc = NULL;

	hres = CoCreateInstance(
			   CLSID_WbemLocator,
			   0,
			   CLSCTX_INPROC_SERVER,
			   IID_IWbemLocator, (LPVOID*) &pLoc);

	if (FAILED(hres))
	{
		_com_issue_error(hres);
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices* pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
			   _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
			   NULL,                    // User name. NULL = current user
			   NULL,                    // User password. NULL = current
			   0,                       // Locale. NULL indicates current
			   NULL,                    // Security flags.
			   0,                       // Authority (e.g. Kerberos)
			   0,                       // Context object
			   &pSvc                    // pointer to IWbemServices proxy
		   );

	if (FAILED(hres))
	{
		pLoc->Release();
		_com_issue_errorex(hres, pLoc, __uuidof(pLoc));
	}


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
			   pSvc,                        // Indicates the proxy to set
			   RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
			   RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
			   NULL,                        // Server principal name
			   RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
			   RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			   NULL,                        // client identity
			   EOAC_NONE                    // proxy capabilities
		   );

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		_com_issue_error(hres);
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
			   bstr_t("WQL"),
			   bstr_t(("SELECT * FROM " + key).c_str()),
			   WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			   NULL,
			   &pEnumerator);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		_com_issue_errorex(hres, pSvc, __uuidof(pSvc));
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject* pclsObj;
	ULONG uReturn = 0;

	std::list< std::string > result;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
									   &pclsObj, &uReturn);

		if(0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
		if (vtProp.vt != VT_EMPTY && vtProp.vt != VT_NULL)
		{
			std::string str = static_cast<const char*>(bstr_t(vtProp));
			StrTrim(&str.front(), " ");
			str = std::string(str.c_str());
			result.push_back(str);
		}
		VariantClear(&vtProp);
	}

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	pclsObj->Release();

	return result;
}
