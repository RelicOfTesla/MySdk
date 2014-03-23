#include "stdafx.h"
#include "atldll_enum.h"
#include <atlbase.h>
#include <sdk/util/codeSearch.h>
#include <sdk/com/util.hpp>
#include <vector>
#include "dbg_assert.h"

struct AtlComInstanceRecord
{
	std::wstring help_name;
	std::wstring disp_name;
	CLSID clsid;
};

struct AtlComInstanceRecordV2 : AtlComInstanceRecord
{
	UINT UnknowVtbl;
	struct iid_record
	{
		IID iid;
		UINT VtblOff;
	};
	typedef std::vector< iid_record > iid_record_list;
	iid_record_list iid;

	AtlComInstanceRecordV2() : UnknowVtbl( 0 )
	{}
};

_ATL_COM_MODULE* GetComModulePtr( lpfn_DllGetClassObject pfn )
{
	dbg_assert( pfn );
	const char* key1 = "B9 ?? ?? ?? ??";
	const BYTE* pMovEcxXXX = CodeSearch( pfn, 0x100, key1 );
	if( pMovEcxXXX )
	{
		void* ptr = * ( void**)( pMovEcxXXX + 1 );
		_ATL_COM_MODULE* pResult = ( _ATL_COM_MODULE*) ptr;
		if( !IsBadReadPtr( pResult, sizeof( _ATL_COM_MODULE ) ) )
		{
			if( abs( ( float ) pResult->cbSize - sizeof( _ATL_COM_MODULE ) ) <= 8 )
			{
				return pResult;
			}
		}
	}
	key1 = "5D E9 ?? ?? ?? ??";
	const BYTE* jmp_xxx = CodeSearch( pfn, 0x100, key1 );
	if( jmp_xxx )
	{
		jmp_xxx = ( const BYTE*) CodeOffsetCalcPtr( jmp_xxx + 1 );
		if( !IsBadReadPtr( jmp_xxx, sizeof( _ATL_COM_MODULE ) ) )
		{
			key1 = "FF 75 10 FF 75 0C FF 75 08 68 ?? ?? ?? ??";
			const BYTE* push_xxx = CodeSearch( jmp_xxx, 0x100, key1 );
			if( push_xxx )
			{
				void* ptr = * ( void**)( push_xxx + 10 );
				_ATL_COM_MODULE* pResult = ( _ATL_COM_MODULE*) ptr;
				if( !IsBadReadPtr( pResult, sizeof( _ATL_COM_MODULE ) ) )
				{
					if( abs( ( float ) pResult->cbSize - sizeof( _ATL_COM_MODULE ) ) <= 8 )
					{
						return pResult;
					}
				}
			}
		}
	}
	key1 = "51 52 68 ?? ?? ?? ??";
	const BYTE* pPushXXX = CodeSearch( pfn, 0x100, key1 );
	if( pPushXXX )
	{
		pPushXXX += 2;
		void* ptr = * ( void**)( pPushXXX + 1 );
		_ATL_COM_MODULE* pResult = ( _ATL_COM_MODULE*) ptr;
		if( !IsBadReadPtr( pResult, sizeof( _ATL_COM_MODULE ) ) )
		{
			if( abs( ( float ) pResult->cbSize - sizeof( _ATL_COM_MODULE ) ) <= 8 )
			{
				return pResult;
			}
		}
	}
	return nullptr;
}

_ATL_INTMAP_ENTRY* GetComIntMapEntry( IUnknown* pObj )
{
	void* pfn_QueryInterface = ** ( void***) pObj;

	const char* key1 = "8B 4D 08 FF 75 0C E8 ?? ?? ?? ??";
	const BYTE* call_xxx = CodeSearch( pfn_QueryInterface, 0x100, key1 );
	if( call_xxx )
	{
		call_xxx += 6;
		call_xxx = ( const BYTE*) CodeOffsetCalcPtr( call_xxx );
		if( !IsBadReadPtr( call_xxx, 0x100 ) )
		{
			key1 = "FF 75 08 68 ?? ?? ?? ??";
			const BYTE* push_xxx = CodeSearch( call_xxx, 0x100, key1 );
			if( push_xxx )
			{
				void* ptr = * ( void**)( push_xxx + 4 );
				if( !IsBadReadPtr( ptr, sizeof( _ATL_INTMAP_ENTRY ) ) )
				{
					return ( _ATL_INTMAP_ENTRY*) ptr;
				}
			}
		}
	}
	key1 = "50 51 68 ?? ?? ?? ?? 52";
	const BYTE* push_xxx = CodeSearch( pfn_QueryInterface, 0x100, key1 );
	if( push_xxx )
	{
		void* ptr = * ( void**)( push_xxx + 3 );
		if( !IsBadReadPtr( ptr, sizeof( _ATL_INTMAP_ENTRY ) ) )
		{
			return ( _ATL_INTMAP_ENTRY*) ptr;
		}
	}
	return nullptr;
}

void GetComIID( IUnknown* pObj, AtlComInstanceRecordV2::iid_record_list& result )
{
	_ATL_INTMAP_ENTRY* pIntMap = GetComIntMapEntry( pObj );
	result.clear();
	if( pIntMap )
	{
		for( ; pIntMap->pFunc; ++pIntMap )
		{
			AtlComInstanceRecordV2::iid_record r;
			r.iid = *pIntMap->piid;
			r.VtblOff = pIntMap->dw;
			result.push_back( r );
		}
	}
}
std::wstring GetComHelpName( _ATL_OBJMAP_ENTRY* pEntry )
{
	if( !pEntry->pfnUpdateRegistry )
	{
		return std::wstring();
	}
	char* key = "8B F0 E8 ?? ?? ?? ?? 8B C6 5E C9 C2 04 00 68";
	const BYTE* pPushXXX = CodeSearch( pEntry->pfnUpdateRegistry, 0x200, key );
	if( pPushXXX )
	{
		pPushXXX += 14;
		const wchar_t* pName = * ( const wchar_t**)( pPushXXX + 1 );
		std::wstring r = pName;
		size_t eof = r.find( L' ' );
		if( eof != std::wstring::npos )
		{
			r.erase( eof, std::wstring::npos );
		}
		return r;
	}
	return std::wstring();
}

std::wstring GetTypeInfoName( ITypeInfo* pTypeInfo )
{
	std::wstring result;
	ITypeLib* pTypeLib = nullptr;
	UINT idx_lib = 0;
	pTypeInfo->GetContainingTypeLib( &pTypeLib, &idx_lib );
	if( pTypeLib )
	{
		BSTR name = nullptr;
		pTypeLib->GetDocumentation( idx_lib, &name, nullptr, nullptr, nullptr );
		result = name;
		SysFreeString( name );
		pTypeLib->Release();
	}
	return result;
}
/*
void EnumTypeInfo(ITypeInfo* pTypeInfo)
{
	std::wstring wname = GetTypeInfoName(pTypeInfo);
	TYPEATTR* pTA = nullptr;
	pTypeInfo->GetTypeAttr(&pTA);
	if (pTA)
	{
		for (UINT i = 0; i < pTA->cImplTypes; ++i)
		{
			HREFTYPE hRefType = 0;
			if( pTypeInfo->GetRefTypeOfImplType(i, &hRefType) == S_OK )
			{
				ITypeInfo* pImplTI = nullptr;
				pTypeInfo->GetRefTypeInfo(hRefType, &pImplTI);
				if (pImplTI)
				{
					EnumTypeInfo(pImplTI);
					pImplTI->Release();
				}
			}
		}
		pTypeInfo->ReleaseTypeAttr(pTA);
	}
}
*/
std::wstring GetComDispName( IUnknown* pObj )
{
	std::wstring result;
	IDispatch* pDisp = nullptr;
	pObj->QueryInterface( IID_IDispatch, ( void**)&pDisp );
	if( pDisp )
	{
		UINT count = 0;
		pDisp->GetTypeInfoCount( &count );
		for( UINT i = 0; i < count; ++i )
		{
			ITypeInfo* pTypeInfo = nullptr;
			HRESULT hr = pDisp->GetTypeInfo( i, 0, &pTypeInfo );
			if( pTypeInfo )
			{
				//EnumTypeInfo(pTypeInfo);
				result = GetTypeInfoName( pTypeInfo );
				pTypeInfo->Release();
			}
			if( result.size() )
			{
				break;
			}
		}
		pDisp->Release();
	}
	return result;
}


void get_atl_info( lpfn_DllGetClassObject pfnCreate )
{
	dbg_assert( pfnCreate );
	_ATL_COM_MODULE* pModule = GetComModulePtr( pfnCreate );
	if( pModule )
	{
		for( _ATL_OBJMAP_ENTRY** iter = pModule->m_ppAutoObjMapFirst; iter != pModule->m_ppAutoObjMapLast; ++iter )
		{
			AtlComInstanceRecordV2 r;
			_ATL_OBJMAP_ENTRY* pEntry = *iter;

			r.clsid = *pEntry->pclsid;

			r.help_name = GetComHelpName( pEntry );
			IClassFactory* pCF = nullptr;
			pfnCreate( *pEntry->pclsid, IID_IClassFactory, ( void**)&pCF );
			IUnknown* pObj = nullptr;
			if( pCF )
			{
				pCF->CreateInstance( 0, IID_IUnknown, ( void**)&pObj );
				pCF->Release();
				pCF = nullptr;
			}

			if( pObj )
			{
				r.disp_name = GetComDispName( pObj );
				r.UnknowVtbl = *( UINT*)pObj;

				GetComIID( pObj, r.iid );

				printf(
					"HelpName=%ws\t"	"DispName=%ws\n"
					"CLSID=%ws\t"	"%ws\n",
					r.help_name.c_str(),
					r.disp_name.c_str(),
					get_guid_wstr( r.clsid ).c_str(),
					get_clsid_name( r.clsid ).c_str()
				);
				for( auto it = r.iid.begin(); it != r.iid.end(); ++it )
				{
					printf(
						"[%04x]\t"	"iid=%ws\t"		"%ws\n",
						it->VtblOff,
						get_guid_wstr( it->iid ).c_str(),
						get_iid_name( it->iid ).c_str()
					);
				}
				pObj->Release();
			}
		}
	}
}

void get_atl_info( const TCHAR* filepath )
{
	HMODULE hLib = LoadLibrary( filepath );
	if( hLib )
	{
		lpfn_DllGetClassObject pfnCreate = ( lpfn_DllGetClassObject ) GetProcAddress( hLib, "DllGetClassObject" );
		if( pfnCreate )
		{
			return get_atl_info( pfnCreate );
		}
	}
}