#pragma once

#include "TUnknown.hpp"

extern HINSTANCE g_hInst;

template<typename base_type, typename interface_type, typename ComRefType = CGlobalComRefCount >
class TDispatch_TypeLib : public TUnknownImpl<base_type, mpl::vector<IUnknown, IDispatch, interface_type>, ComRefType>
{
protected:
	ITypeInfo* m_pTypeInfo;
public:
	TDispatch_TypeLib() : m_pTypeInfo( 0 )
	{}
	~TDispatch_TypeLib()
	{
		if( m_pTypeInfo )
		{
			m_pTypeInfo->Release();
		}
	}

	STDMETHOD( GetTypeInfoCount )( UINT* pctinfo )
	{
		ITypeInfo* pTypeInfo = nullptr;
		HRESULT hr = GetTypeInfo( 0, 0, &pTypeInfo );
		if( pTypeInfo )
		{
			*pctinfo = 1;
			pTypeInfo->Release();
			return S_OK;
		}
		return hr;
	}

	STDMETHOD( GetTypeInfo )( UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo )
	{
		HRESULT hr = E_NOTIMPL;
		if( m_pTypeInfo )
		{
			ITypeLib* pTypeLib = nullptr;
			hr = QueryTypeLib( g_hInst, &pTypeLib );
			if( !pTypeLib )
			{
				hr = LoadRegTypeLib( __uuidof( interface_type ), 0,  0, 0, &pTypeLib );
			}
			if( pTypeLib )
			{
				hr = pTypeLib->GetTypeInfoOfGuid( __uuidof( interface_type ), &m_pTypeInfo );
			}
		}
		if( m_pTypeInfo )
		{
			*ppTInfo = m_pTypeInfo;
			m_pTypeInfo->AddRef();
			return S_OK;
		}
		return hr;
	}

	STDMETHOD( GetIDsOfNames )( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId )
	{
		ITypeInfo* pTypeInfo = nullptr;
		GetTypeInfo( 0, 0, &pTypeInfo );
		if( pTypeInfo )
		{
			return DispGetIDsOfNames( pTypeInfo, rgszNames, cNames, rgDispId );
			pTypeInfo->Release();
		}
		return E_NOTIMPL;
	}

	STDMETHOD( Invoke )( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
						 DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr )
	{
		ITypeInfo* pTypeInfo = nullptr;
		GetTypeInfo( 0, 0, &pTypeInfo );
		if( pTypeInfo )
		{
			return DispInvoke( this, pTypeInfo, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
			pTypeInfo->Release();
		}
		return E_NOTIMPL;
	}

protected:
	static HRESULT QueryTypeLib( HMODULE hModule, ITypeLib** pptlib )
	{
		WCHAR szSelfPath[MAX_PATH];
		GetModuleFileNameW( hModule, szSelfPath, MAX_PATH );
		static ITypeLib* g_pTLIB = nullptr;
		HRESULT hr = LoadTypeLib( szSelfPath, &g_pTLIB );
		*pptlib = g_pTLIB;
		return hr;
	}

};