#pragma once

#include <string>
#include "dbg_assert.h"

inline
ULONG GetComRefCount( IUnknown* p )
{
	ULONG r = p->AddRef() - 1;
	p->Release();
	return r;
}


static
void enumTypeInfo( ITypeInfo* pTypeInfo, int dbg_deep = 0 )
{
	dbg_assert( pTypeInfo );
	{
		ITypeLib* pTypeLib = nullptr;
		UINT Index = 0;
		pTypeInfo->GetContainingTypeLib( &pTypeLib, &Index );
		if( pTypeLib )
		{

			BSTR bTypeName = nullptr;
			pTypeLib->GetDocumentation( Index, &bTypeName, nullptr, nullptr, nullptr );
			if( bTypeName )
			{
				for( int i = 0; i < dbg_deep; ++i )
				{
					dbg_printf( "\t" );
				}
				dbg_printf( "%ws\n", bTypeName );
				SysFreeString( bTypeName );
			}
			pTypeLib->Release();
		}
	}

	TYPEATTR* pTA = nullptr;
	pTypeInfo->GetTypeAttr( &pTA );
	if( pTA )
	{
		for( int iImpl = 0; iImpl < pTA->cImplTypes; ++iImpl )
		{
			HREFTYPE hRefType = 0;
			pTypeInfo->GetRefTypeOfImplType( iImpl, &hRefType );
			ITypeInfo* pImplTypeInfo = nullptr;
			pTypeInfo->GetRefTypeInfo( hRefType, &pImplTypeInfo );
			if( pImplTypeInfo )
			{
				enumTypeInfo( pImplTypeInfo, dbg_deep + 1 );
				pImplTypeInfo->Release();
			}
		}

		pTypeInfo->ReleaseTypeAttr( pTA );
	}
}

static
void enumTypeLib( ITypeLib* pTypeLib )
{
	dbg_assert( pTypeLib );
	TLIBATTR* pLTA = nullptr;
	pTypeLib->GetLibAttr( &pLTA );
	if( pLTA )
	{
		pTypeLib->ReleaseTLibAttr( pLTA );
	}
	UINT tc = pTypeLib->GetTypeInfoCount();
	for( UINT i = 0; i < tc; ++i )
	{
		ITypeInfo* pTypeInfo = nullptr;
		pTypeLib->GetTypeInfo( i, &pTypeInfo );
		if( pTypeInfo )
		{
			enumTypeInfo( pTypeInfo );
			pTypeInfo->Release();
		}
	}

}

static void enumTypeInfo( IDispatch* pDisp )
{
	dbg_assert( pDisp );
	UINT count = 0;
	pDisp->GetTypeInfoCount( &count );
	for( UINT i = 0; i < count; ++i )
	{
		ITypeInfo* pTI = nullptr;
		pDisp->GetTypeInfo( i, 0, &pTI );
		if( pTI )
		{
			enumTypeInfo( pTI );
		}
	}
}