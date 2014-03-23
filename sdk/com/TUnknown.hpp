#pragma once

#include "ComRef.hpp"
#include "../mini_mpl.hpp"
#include <sdk/noncopyable.h>
#include <c++10/type_traits.h>
#include <sdk/dbg_assert.h>

namespace detail
{
	template < typename V, typename dest_type, typename I = mpl::int_<0> >
	struct get_first_convert_but_not_same_type
	{
		typedef typename mpl::at<V, I>::type cur_type;
		enum
		{
			is_same = std::is_same<cur_type , dest_type>::value,
			is_convert = std::is_convertible<cur_type , dest_type>::value,
		};

		typedef typename get_first_convert_but_not_same_type < V, dest_type, typename mpl::next<I>::type > next_type;

		typedef typename std::conditional < is_convert && !is_same,
				cur_type,
				typename next_type::type
				>::type type;
	};

	template<typename V, typename dest_type>
	struct get_first_convert_but_not_same_type < V, dest_type, mpl::int_ < mpl::size<V>::value - 1 > >
	{
		typedef mpl::na type;
	};

	template<typename V, typename T>
	struct get_com_cast_type
	{
		typedef typename get_first_convert_but_not_same_type<V, T>::type safe_first_type;

		typedef safe_first_type safe_type;
		typedef T  notsafe_type;
		enum
		{
			has_safe = !std::is_same<safe_type, mpl::na>::value
		};
		typedef typename std::conditional <has_safe, safe_type, notsafe_type>::type type;
	};

	template<typename V, typename I>
	struct query_interface_t
	{
		typedef typename mpl::at<V, I>::type current_type;
		typedef typename get_com_cast_type<V, current_type>::type cast_type;

		template<typename U>
		static inline HRESULT invoke( U* pThis, REFIID iid, void** ppv )
		{
			if( iid == __uuidof( current_type ) )
			{
				cast_type* p = pThis;
				*ppv = p;
				p->AddRef();
				return S_OK;
			}
			return query_interface_t< V, mpl::next<I>::type >::invoke( pThis, iid, ppv );
		}
	};

	template<typename V>
	struct query_interface_t < V, mpl::int_ < mpl::size<V>::value - 1 > >
	{
		typedef mpl::int_ < mpl::size<V>::value - 1 > I;
		typedef typename mpl::at<V, I>::type current_type;
		typedef typename get_com_cast_type<V, current_type>::type cast_type;

		template<typename U>
		static inline HRESULT invoke( U* pThis, REFIID iid, void** ppv )
		{
			if( iid == __uuidof( current_type ) )
			{
				cast_type* p = pThis;
				*ppv = p;
				p->AddRef();
				return S_OK;
			}
			return E_NOINTERFACE;
		}
	};
};

template < typename base_type, typename iid_list /*= mpl::vector<IUnknown>*/, typename ComCountType = CGlobalComRefCount >
class TUnknownImpl : public base_type, noncopyable
{
	typedef detail::query_interface_t<iid_list, mpl::int_<0> > query_interface_t;
protected:
	ComCountType m_RefCount;
protected:
	virtual ~TUnknownImpl() {}
public:
	STDMETHOD_( ULONG, AddRef )()
	{
		return m_RefCount.Inc();
	}
	STDMETHOD_( ULONG, Release )()
	{
		ULONG r = m_RefCount.Dec();
		if( r == 0 )
		{
			delete this;
		}
		return r;
	}
	STDMETHOD( QueryInterface )( REFIID iid, void** ppv )
	{
		if( !ppv )
		{
			return E_INVALIDARG;
		}
		return query_interface_t::invoke( this, iid, ppv );
	}
};

//////////////////////////////////////////////////////////////////////////
struct IWeakObject : IUnknown
{
	STDMETHOD( Look )( REFIID , OUT void**) = 0;
};
struct DECLSPEC_UUID( "{92FD3709-37CE-4C9F-885C-14B51E6AC663}" ) IWeakObject;

#define IWeakObject_(x) IWeakObject

namespace detail
{
	template<typename ComCountType>
	struct TComWeakCount :
			TUnknownImpl< IWeakObject, mpl::vector<IUnknown, IWeakObject> >,
			ComCountType // ,noncopyable
	{
		TComWeakCount() : m_rawptr( nullptr )
		{}
		void inline InitObj( IUnknown* pv )
		{
			m_rawptr = pv;
		}

		STDMETHOD( Look )( REFIID iid, OUT void** ppv )
		{
			HRESULT hr = E_FAIL;
			if( Inc() > 1 )
			{
				hr = m_rawptr->QueryInterface( iid, ppv );
				m_rawptr->Release();
			}
			else
			{
				Dec();
			}
			return hr;
		}
	protected:
		IUnknown* m_rawptr;
	};

	template<typename ComCountType>
	struct TAutoComWeakCount : noncopyable
	{
	public:
		TComWeakCount<ComCountType>* m_obj;

		TAutoComWeakCount()
		{
			m_obj = new TComWeakCount<ComCountType>;
		}
		~TAutoComWeakCount()
		{
			m_obj->Release();
		}

		ULONG Inc()
		{
			return m_obj->Inc();
		}
		ULONG Dec()
		{
			return m_obj->Dec();
		}

		TComWeakCount<ComCountType>* operator ->()const
		{
			return m_obj;
		}
	};

};


template < typename base_type, typename iid_list /*= mpl::vector<IUnknown>*/, typename ComCountType = CGlobalComRefCount,
		   bool WeakObject_In_QueryInterface = true  >
struct TWeakUnknownImpl : TUnknownImpl<base_type, iid_list, detail::TAutoComWeakCount<ComCountType> >
{
	TWeakUnknownImpl()
	{
		IUnknown* pUnk = nullptr;
		this->QueryInterface( IID_IUnknown, ( void**)&pUnk );
		dbg_assert( pUnk );
		if( pUnk )
		{
			m_RefCount->InitObj( pUnk );
			pUnk->Release();
		}
	}

	STDMETHOD( QueryInterface )( REFIID iid, void** ppv )
	{
		HRESULT hr = __super::QueryInterface( iid, ppv );
		if( WeakObject_In_QueryInterface )
		{
			if( hr != S_OK )
			{
				if( iid == __uuidof( IWeakObject ) )
				{
					// Not AddRef This Object Count
					// Is AddRef WeakObject Count
					return GetWeak( ( IWeakObject**)ppv );  //return m_RefCount->QueryInterface(__uuidof(IWeakObject), ppv);
				}
			}
		}
		return hr;
	}

	inline HRESULT GetWeak( IWeakObject** ppv )
	{
		return m_RefCount->QueryInterface( __uuidof( IWeakObject ), ( void**)ppv );
	}
};
