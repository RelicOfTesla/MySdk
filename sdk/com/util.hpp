#pragma once

#include <sdk/shared_ptr.h>
#include <atlcomcli.h>
#include "ComPtr.hpp"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x){ x->Release(); x = nullptr; }
#endif

template<typename T>
static inline void _com_release( T* p )
{
	if( p )
	{
		static_cast<IUnknown*>( p )->Release();
	}
}
template<typename T>
inline shared_ptr<T> SharedComPtr_Attach( T* p )
{
	shared_ptr<T> sp( p, _com_release<T> );
	return sp;
}
template<typename T>
inline shared_ptr<T> SharedComPtr_CopyFrom( T* p )
{
	static_cast<IUnknown*>( p )->AddRef();
	shared_ptr<T> sp( p, _com_release<T> );
	return sp;
}

template<typename T>
inline CComPtr<T> AtlComPtr_Attach( T* p )
{
	CComPtr<T> sp;
	sp.Attach( p );
	return sp;
}
template<typename T>
inline CComPtr<T> AtlComPtr_CopyFrom( T* p )
{
	CComPtr<T> sp = p;
	return sp;
}


#define comptr_attach SharedComPtr_Attach
#define comptr_copyfrom SharedComPtr_CopyFrom



inline
size_t GetSafeArrayBufferSize( SAFEARRAY* psa )
{
	LONG ub = 0, lb = 0;
	long n = SafeArrayGetDim( psa );
	SafeArrayGetUBound( psa, n, &ub );
	SafeArrayGetLBound( psa, n, &lb );
	return ub - lb + 1;
}


static
std::wstring get_guid_wstr( REFIID id )
{
	std::wstring val;
	LPOLESTR pName = nullptr;
	StringFromIID( id, &pName );
	val = pName;
	CoTaskMemFree( pName );
	return val;
}

static
std::wstring get_iid_name( REFIID iid )
{
	std::wstring result;
	std::wstring path = L"\\Interface\\" + get_guid_wstr( iid );

	HKEY hKey = 0;
	RegOpenKeyW( HKEY_CLASSES_ROOT, path.c_str(), &hKey );
	if( hKey )
	{
		WCHAR data[255] = {0};
		LONG bytes = sizeof( data );
		RegQueryValueW( hKey, L"", data, &bytes );
		RegCloseKey( hKey );
		result = data;
	}
	return result;
}

static
std::wstring get_clsid_name( REFCLSID clsid )
{
	std::wstring result;
	std::wstring path = L"\\CLSID\\" + get_guid_wstr( clsid );

	HKEY hKey = 0;
	RegOpenKeyW( HKEY_CLASSES_ROOT, path.c_str(), &hKey );
	if( hKey )
	{
		WCHAR data[255] = {0};
		LONG bytes = sizeof( data );
		RegQueryValueW( hKey, L"", data, &bytes );
		RegCloseKey( hKey );
		result = data;
	}
	return result;
}