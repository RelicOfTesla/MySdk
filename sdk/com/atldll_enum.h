#pragma once


typedef HRESULT( STDMETHODCALLTYPE* lpfn_DllGetClassObject )( REFCLSID rclsid, REFIID riid, LPVOID* ppv );

void get_atl_info( lpfn_DllGetClassObject pfnCreate );
void get_atl_info( const TCHAR* filepath );