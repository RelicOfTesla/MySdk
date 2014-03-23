#pragma once


static IUnknown* DllCreateInstance( LPCTSTR filepath, REFCLSID clsid, REFIID iid )
{
	HMODULE hInst = GetModuleHandle( filepath );
	BOOL bFree = FALSE;
	if( !hInst )
	{
		hInst = LoadLibrary( filepath );
		bFree = TRUE;
	}
	if( hInst )
	{
		typedef HRESULT( _stdcall * lpfn_DllGetClassObject )( REFCLSID, REFIID, LPVOID*);

		if( lpfn_DllGetClassObject f = ( lpfn_DllGetClassObject )GetProcAddress( hInst, "DllGetClassObject" ) )
		{
			try
			{
				IClassFactory* pCF = nullptr;
				f( clsid, IID_IClassFactory, ( void**)&pCF );
				if( pCF )
				{
					IUnknown* p = nullptr;
					pCF->CreateInstance( nullptr, iid, ( void**)&p );
					pCF->Release();
					if( p )
					{
						return p;
					}
				}
			}
			catch( ... )
			{}
		}
		if( bFree )
		{
			FreeLibrary( hInst );
		}
	}
	return nullptr;
}
template<typename R> inline R* DllCreateInstance( LPCTSTR filepath, REFCLSID clsid )
{
	return ( R*)DllCreateInstance( filepath, clsid, __uuidof( R ) );
}

static BOOL DllComUnload( HMODULE& hInst )
{
	if( hInst )
	{
		if( FARPROC f = GetProcAddress( hInst, "DllCanUnloadedNow" ) )
		{
			if( f() == S_OK )
			{
				FreeLibrary( hInst );
				hInst = nullptr;
				return TRUE;
			}
		}
	}
	return FALSE;
}
