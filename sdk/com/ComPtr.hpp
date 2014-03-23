#pragma once

#include "dbg_assert.h"

namespace detail
{
	template <class T>
	class _NoAddRefReleaseCom :
		public T
	{
	private:
		STDMETHOD_( ULONG, AddRef )() = 0;
		STDMETHOD_( ULONG, Release )() = 0;
	};


	static inline IUnknown* ComPtrAssign( IUnknown** pp, IUnknown* lp )
	{
		if( pp == NULL )
		{
			return NULL;
		}

		if( lp != NULL )
		{
			lp->AddRef();
		}
		if( *pp )
		{
			( *pp )->Release();
		}
		*pp = lp;
		return lp;
	}

	static inline IUnknown* ComQIPtrAssign( IUnknown** pp, IUnknown* lp, REFIID riid )
	{
		if( pp == NULL )
		{
			return NULL;
		}

		IUnknown* pTemp = *pp;
		*pp = NULL;
		if( lp != NULL )
		{
			lp->QueryInterface( riid, ( void**)pp );
		}
		if( pTemp )
		{
			pTemp->Release();
		}
		return *pp;
	}
};

template <class T>
class ComPtr
{

public:
	T* m_ptr;

public:
	ComPtr()
	{
		m_ptr = NULL;
	}
	ComPtr( T* lp )
	{
		m_ptr = lp;

		if( m_ptr != NULL )
		{
			m_ptr->AddRef();
		}
	}
	~ComPtr() throw()
	{
		if( m_ptr )
		{
			m_ptr->Release();
		}
	}
	operator T* () const throw()
	{
		return m_ptr;
	}
	T& operator*() const
	{
		dbg_assert( m_ptr != NULL );
		return *m_ptr;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
		dbg_assert( m_ptr == NULL );
		return &m_ptr;
	}
	detail::_NoAddRefReleaseCom<T>* operator->() const throw()
	{
		dbg_assert( m_ptr != NULL );
		return ( detail::_NoAddRefReleaseCom<T>*)m_ptr;
	}
	bool operator!() const throw()
	{
		return ( m_ptr == NULL );
	}
	bool operator<( T* pT ) const throw()
	{
		return m_ptr < pT;
	}
	bool operator!=( T* pT ) const
	{
		return !operator==( pT );
	}
	bool operator==( T* pT ) const throw()
	{
		return m_ptr == pT;
	}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = m_ptr;

		if( pTemp )
		{
			m_ptr = NULL;
			pTemp->Release();
		}
	}
	// Compare two objects for equivalence
	bool IsEqualObject( _Inout_opt_ IUnknown* pOther ) throw()
	{
		if( m_ptr == NULL && pOther == NULL )
		{
			return true;    // They are both NULL objects
		}

		if( m_ptr == NULL || pOther == NULL )
		{
			return false;    // One is NULL the other is not
		}

		ComPtr<IUnknown> punk1;
		ComPtr<IUnknown> punk2;
		m_ptr->QueryInterface( __uuidof( IUnknown ), ( void**)&punk1 );
		pOther->QueryInterface( __uuidof( IUnknown ), ( void**)&punk2 );
		return punk1 == punk2;
	}
	// Attach to an existing interface (does not AddRef)
	void Attach( _In_opt_ T* p2 ) throw()
	{
		if( m_ptr )
		{
			m_ptr->Release();
		}

		m_ptr = p2;
	}
	// Detach the interface (does not Release)
	T* Detach() throw()
	{
		T* pt = m_ptr;
		m_ptr = NULL;
		return pt;
	}

	HRESULT CoCreateInstance(
		IN REFCLSID rclsid,
		IN OUT OPTIONAL LPUNKNOWN pUnkOuter = NULL,
		IN DWORD dwClsContext = CLSCTX_ALL ) throw()
	{
		dbg_assert( p == NULL );
		return ::CoCreateInstance( rclsid, pUnkOuter, dwClsContext, __uuidof( T ), ( void**)&m_ptr );
	}
	HRESULT CoCreateInstance(
		IN LPCOLESTR szProgID,
		IN OUT OPTIONAL LPUNKNOWN pUnkOuter = NULL,
		IN DWORD dwClsContext = CLSCTX_ALL ) throw()
	{
		CLSID clsid;
		HRESULT hr = CLSIDFromProgID( szProgID, &clsid );
		dbg_assert( p == NULL );

		if( SUCCEEDED( hr ) )
		{
			hr = ::CoCreateInstance( clsid, pUnkOuter, dwClsContext, __uuidof( T ), ( void**)&m_ptr );
		}

		return hr;
	}
	template <class Q>
	HRESULT QueryInterface( OUT Q** pp ) const throw()
	{
		dbg_assert( pp != NULL );
		return m_ptr->QueryInterface( __uuidof( Q ), ( void**)pp );
	}


	T* operator=( IN T* lp ) throw()
	{
		if( *this != lp )
		{
			return static_cast<T*>( detail::ComPtrAssign( ( IUnknown**)&m_ptr, lp ) );
		}

		return *this;
	}
	template <typename Q>
	T* operator=( _Inout_ const ComPtr<Q>& lp ) throw()
	{
		if( !IsEqualObject( lp ) )
		{
			return static_cast<T*>( detail::ComQIPtrAssign( ( IUnknown**)&m_ptr, lp, __uuidof( T ) ) );
		}

		return *this;
	}
	T* operator=( _Inout_ const ComPtr<T>& lp ) throw()
	{
		if( *this != lp )
		{
			return static_cast<T*>( detail::ComPtrAssign( ( IUnknown**)&m_ptr, lp ) );
		}

		return *this;
	}
	ComPtr( _Inout_ ComPtr<T>&& lp ) throw()
	{
		m_ptr = lp.m_ptr;
		lp.m_ptr = NULL;
	}
	T* operator=( _Inout_ ComPtr<T> && lp ) throw()
	{
		if( *this != lp )
		{
			if( m_ptr != NULL )
			{
				m_ptr->Release();
			}

			m_ptr = lp.m_ptr;
			lp.m_ptr = NULL;
		}

		return *this;
	}

};

template<typename T>
struct com_auto_ptr
{
	com_auto_ptr( T* p, bool addref )
	{
		m_p = p;
		if( addref && p )
		{
			p->AddRef();
		}
	}
	~com_auto_ptr()
	{
		if( m_p )
		{
			m_p->Release();
		}
	}

	com_auto_ptr( const com_auto_ptr<T>& r )
	{
		m_p = r.m_p;
		if( m_p )
		{
			m_p->AddRef();
		}
	}

	void operator = ( const com_auto_ptr<T>& r )
	{
		if( r.m_p )
		{
			r.m_p->AddRef();
		}
		if( m_p )
		{
			m_p->Release();
		}
		m_p = r.m_p;
	}

	T* operator ->()const
	{
		return m_p;
	}

// 	T** operator &()
// 	{
// 		return &m_p;
// 	}

	operator T* ()const
	{
		return m_p;
	}

	T& operator*()
	{
		return *m_p;
	}

	T* m_p;
};