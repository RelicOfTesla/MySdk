#pragma once

#include <sdk/shared_ptr.h>
#include <atlcomcli.h>
#include <comip.h>
#include <sdk/expand_macro.h>

template<typename T, typename sp_t>
struct TBasePtrConvert
{
	typedef sp_t SP;
	typedef SP auto_ptr;
	static auto_ptr Attach( T* p );
	static auto_ptr	CopyFrom( T* p );
};
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct TRawComPtrConvert : TBasePtrConvert< T, T* >
{
	static inline T* Attach( T* p )
	{
		return p;
	}
	static inline T* CopyFrom( T* p )
	{
		static_cast<IUnknown*>( p )->AddRef();
		return p;
	}
};
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct TAtlComPtrCast : TBasePtrConvert< T, CComPtr<T> >
{
	static inline SP Attach( T* p )
	{
		SP r;
		r.Attach( p );
		return r;
	}
	static inline SP CopyFrom( T* p )
	{
		return SP( p );
	}
};


template<typename T> inline
CComPtr<T>	AtlComPtr_Attach( T* p )
{
	return TAtlComPtrCast<T>::Attach( p );
}
//////////////////////////////////////////////////////////////////////////
template<typename T, const GUID* iid = &__uuidof( T ) >
struct TComPtrCast : TBasePtrConvert< T, _com_ptr_t< _com_IIID<T, iid> > >
{
	static inline SP Attach( T* p )
	{
		return SP( p, false );
	}
	static inline SP CopyFrom( T* p )
	{
		return SP( p );
	}
};
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct TSharedComPtrCast : TBasePtrConvert< T, shared_ptr<T> >
{
	static void _ComPtr_Release( T* p )
	{
		static_cast<IUnknown*>( p )->Release();
	}
	//////////////////////////////////////////////////////////////////////////
	static inline SP Attach( T* p )
	{
		return SP( p, _ComPtr_Release );
	}

	static inline shared_ptr<T> CopyFrom( T* p )
	{
		static_cast<IUnknown*>( p )->AddRef();
		return SP( p, _ComPtr_Release );
	}
};
//////////////////////////////////////////////////////////////////////////
namespace _
{
	template<typename object_type, typename AutoPtrConvert >
	struct object_ptr : AutoPtrConvert
	{
		typedef typename AutoPtrConvert::SP SP;
		typedef typename AutoPtrConvert::auto_ptr auto_ptr;

		static inline SP _Attach( object_type* ptr )
		{
			return __super::Attach( ptr );
		}

		static SP Create()
		{
			return _Attach( new object_type() );
		}

#pragma push_macro("DEF_FUNCTION")
#undef DEF_FUNCTION
#define DEF_FUNCTION(_m, N, _d) \
		template< __any_typename_T(N) > \
		static SP Create( __any_params_t(N) ) \
		{ \
			return _Attach( new object_type( __any_args_t(N) ) ); \
		}

		__expand_repeat_from_to( 1, 9, );

#pragma pop_macro("DEF_FUNCTION")
	};

};
