#pragma once

template<int nInitValue>
class CRefCountImpl
{
public:
	CRefCountImpl() : m_ref( nInitValue )
	{}
	long Inc()
	{
		return InterlockedIncrement( &m_ref );
	}
	long Dec()
	{
		return InterlockedDecrement( &m_ref );
	}
	long Get()const
	{
		return m_ref;
	}
private:
	long m_ref;
private:
	CRefCountImpl( const CRefCountImpl&) {}
	void operator = ( const CRefCountImpl&) {}
};

class CComRefCount : public CRefCountImpl<1>
{
};

class CGlobalComRefCount : public CComRefCount
{
public:
	static CRefCountImpl<0> g_ModuleComCount;
	static HRESULT DllCanUnloadedNow()
	{
		return g_ModuleComCount.Get();
	}

	CGlobalComRefCount()
	{
		g_ModuleComCount.Inc();
	}
	~CGlobalComRefCount()
	{
		g_ModuleComCount.Dec();
	}
};
