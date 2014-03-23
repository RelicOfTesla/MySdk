#pragma once

#include "TUnknown.hpp"

template<typename ImplType, typename InterfaceType = ImplType>
struct TClassFactory : IClassFactory
{
	typedef mpl::vector<IUnknown, IClassFactory> iid_list;

	STDMETHOD( CreateInstance )( IUnknown* pUnkOuter, REFIID riid, void** ppvObject )
	{
		InterfaceType* pInterface = CreateInterface();
		if( pInterface )
		{
			HRESULT hr = pInterface->QueryInterface( riid, ppvObject );
			pInterface->Release();
			return hr;
		}
		return E_OUTOFMEMORY;
	}
	STDMETHOD( LockServer )( BOOL b )
	{
		return E_NOTIMPL;
	}

	static InterfaceType* CreateInterface()
	{
		return new ImplType;
	}

	static IClassFactory* CreateObject()
	{
		return new TUnknownImpl< TClassFactory<ImplType, InterfaceType>, mpl::vector<IUnknown, IClassFactory> >;
	}
};
