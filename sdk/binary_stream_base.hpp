#pragma once

#include "binary.h"
#include "noncopyable.h"

template<typename parent_type>
class memory_istream_impl : noncopyable
{
public:
	memory_istream_impl( int offset = 0 ) : m_read_offset( offset )
	{}

	void pop_buffer( BYTE* buf, size_t len )
	{
		if( m_read_offset + len > this->max_size() )
		{
			throw std::out_of_range( "out memory range" );
		}

		memcpy( buf, this->c_data() + m_read_offset, len );
		m_read_offset += len;
	}

	bool is_eof() const
	{
		return !has_buffer( 1 );
	}
	bool has_buffer( size_t request_length ) const
	{
		return m_read_offset + request_length <= max_size();
	}
	bool peek( size_t len )
	{
		if( has_buffer( len ) )
		{
			m_read_offset += len;
			return true;
		}

		return false;
	}
	size_t get_offset() const
	{
		return m_read_offset;
	}

	inline const BYTE* c_data() const
	{
		return parent_type::c_data( this );
	}
	inline size_t max_size() const
	{
		return parent_type::max_size( this );
	}
protected:
	size_t m_read_offset;
};

class memory_istream : public memory_istream_impl<memory_istream>
{
	typedef memory_istream_impl<memory_istream> super_type;
public:
	memory_istream( const void* data, size_t len ) : m_ptr( ( BYTE*)data ), m_maxlen( len )
	{}


	static inline const BYTE* c_data( const super_type* p )
	{
		return ( ( memory_istream*)p )->m_ptr;
	}
	static inline size_t max_size( const super_type* p )
	{
		return ( ( memory_istream*)p )->m_maxlen;
	}
protected:
	const BYTE* m_ptr;
	size_t m_maxlen;
};

class binary_istream_ref : public memory_istream_impl<binary_istream_ref>
{
	typedef memory_istream_impl<binary_istream_ref> super_type;
public:
	binary_istream_ref( const binary& bin, int read_offset = 0 ) :  m_ref_data( bin ), super_type( read_offset )
	{}

	static inline const BYTE* c_data( const super_type* p )
	{
		return &( ( ( binary_istream_ref*)p )->m_ref_data[0] );
	}
	static inline size_t max_size( const super_type* p )
	{
		return ( ( binary_istream_ref*)p )->m_ref_data.size();
	}
protected:
	const binary& m_ref_data;
};
struct binary_istream_new : binary_istream_ref
{
public:
	binary_istream_new() : binary_istream_ref( m_loc_data )
	{}
	void clear()
	{
		m_loc_data.clear();
		m_read_offset = 0;
	}
	void* resize_buffer( size_t n )
	{
		m_loc_data.resize( n );
		return &m_loc_data[0];
	}
protected:
	binary m_loc_data;
};

//////////////////////////////////////////////////////////////////////////
struct binary_ostream_ref : noncopyable
{
	binary_ostream_ref( binary& bin ) : m_data( bin )
	{}

	void append( const BYTE* pd, size_t len )
	{
		binary_append( m_data, pd, len );
	}

	binary& m_data;
};

struct binary_ostream_new : noncopyable
{
	binary_ostream_new()
	{}

	void append( const BYTE* pd, size_t len )
	{
		binary_append( m_data, pd, len );
	}

	binary m_data;
};

//////////////////////////////////////////////////////////////////////////
struct binptr_ostream : noncopyable
{
	binptr_ostream( binptr pbin = binptr( new binary ) ) : m_pBin( pbin )
	{}

	void append( const BYTE* pd, size_t len )
	{
		binptr_append( m_pBin, pd, len );
	}
	binptr get()
	{
		return m_pBin;
	}

	binptr m_pBin;
};
