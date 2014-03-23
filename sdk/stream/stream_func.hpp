#pragma once

#include "binary_stream.hpp"
#include "stream_call.hpp"
#include "atom.hpp"
#include <boost/thread/tss.hpp>
#include "dbg_assert.h"


struct session_data
{
	typedef long session_id;

	session_id m_new_session_id;
	session_id m_remote_session_id;
};

extern long g_atoms_session;


struct recv_session : session_data
{
	session_id get_session_id() const	// last request id
	{
		return m_remote_session_id;
	}
};

struct send_session : session_data
{
	send_session()
	{
		static_assert( sizeof( atom_make( &g_atoms_session ) ) == sizeof( session_id ), "session_id size error" ) ;
		m_new_session_id = atom_make( &g_atoms_session );
		m_remote_session_id = 0;
	}
	send_session( const recv_session& req )
	{
		static_assert( sizeof( atom_make( &g_atoms_session ) ) == sizeof( session_id ), "session_id size error" ) ;
		m_new_session_id = atom_make( &g_atoms_session );
		m_remote_session_id = req.m_new_session_id;
	}

	session_id get_session_id() const	// local new request id
	{
		return m_new_session_id;
	}
};


struct function_ipackage : recv_session
{
	std::string get_function_name()const
	{
		return m_funcname;
	}

	template<typename Archive>
	static inline void serialize_load( Archive& ar, function_ipackage& fpk )
	{
		USHORT max_len = 0;
		ar >> max_len;
		ar >> pod_buffer( fpk.m_istm.resize_buffer( max_len ),  max_len );

		session_data* psd = &fpk;
		fpk >> *psd >> fpk.m_funcname;
	}
protected:
	local_binary_istream m_istm;
	std::string m_funcname;

	friend struct pod_archive<function_ipackage>;
};


struct function_opackage : send_session
{
	function_opackage( const std::string& funcname )
	{
		session_data* psd = this;
		m_ostm << *psd << funcname;
	}

	function_opackage( const std::string& funcname, const function_ipackage& req )
		: send_session( req )
	{
		session_data* psd = this;
		m_ostm << *psd << funcname;
	}

	template<typename Archive>
	static inline void serialize_save( Archive& ar, function_opackage& fpk )
	{
		ar << USHORT( fpk.m_ostm.m_data.size() );
		ar << pod_const_data( fpk.m_ostm.m_data.data(), fpk.m_ostm.m_data.size() );
	}
protected:
	binary_ostream m_ostm;

	friend struct pod_archive<function_opackage>;
};

template<>
struct pod_archive<function_ipackage>
{
	static void invoke( function_ipackage& fpk, pod_buffer& buf )
	{
		fpk.m_istm >> buf;
	}
	// 	static void invoke( function_ipackage& fpk, const pod_const_data& cdata )
	// 	{
	// 		fpk.ostm << cdata;
	// 	}
};

template<>
struct pod_archive<function_opackage>
{
	// 	static void invoke( function_opackage& fpk, pod_buffer& buf )
	// 	{
	// 		fpk.istm >> buf;
	// 	}
	static void invoke( function_opackage& fpk, const pod_const_data& cdata )
	{
		fpk.m_ostm << cdata;
	}
};

template<typename Stream>
struct serialize_t<Stream, std::string> : serialize_load_save<Stream, std::string>
{
	template<typename Archive>
	static inline void load( Archive& ar, std::string& str )
	{
		USHORT len;
		ar >> len;
		str.resize( len );
		ar >> pod_buffer( &str.front(), len );
	}
	template<typename Archive>
	static inline void save( Archive& ar, std::string& str )
	{
		dbg_assert( str.size() <= 0xffff );
		ar << USHORT( str.size() )
		   << pod_const_data( str.c_str(), str.size() );
	}
};

//////////////////////////////////////////////////////////////////////////


template<typename type, int GI = 0>
struct thread_var_tmpref
{
	typedef const type& ref_type;
	typedef const type* ptr_type;
	typedef boost::thread_specific_ptr< ptr_type > instance_type;
	static instance_type g_last;

	static inline void set( ref_type& v )
	{
		if( !g_last.get() )
		{
			g_last.reset( new ptr_type );
		}

		*g_last = &v;
	}

	static inline ptr_type get()
	{
		ptr_type* pp = g_last.get();

		if( !pp )
		{
			throw std::logic_error( "p == null" );
		}

		return *pp;
	}
};

typedef thread_var_tmpref<function_ipackage> current_function_ipackage;

template<typename Function>
struct prev_stream_invoke<function_ipackage, Function>
{
	static inline void invoke( function_ipackage& stm, const Function& func )
	{
		current_function_ipackage::set( stm );
	}
};


//////////////////////////////////////////////////////////////////////////

template<typename Stream>
struct serialize_t<Stream, function_ipackage> : serialize_load_save<Stream, function_ipackage>
{
	template<typename Archive>
	static void load( Archive& ar, function_ipackage& fpk )
	{
		function_ipackage::serialize_load( ar, fpk );
	}
	template<typename Archive>
	static void save( Archive& ar, function_ipackage& fpk )
	{
		static_assert( 0, "error" );
	}
};

/*
enum
{
	CMD_STRCMD = 1,
};

template<typename Stream>
struct serialize_t<Stream, function_opackage> : serialize_load_save<Stream, function_opackage>
{
	template<typename Archive>
	static void load( Archive& ar, function_opackage& fpk )
	{
		static_assert( 0, "error" );
	}
	template<typename Archive>
	static void save( Archive& ar, function_opackage& fpk )
	{
		ar << USHORT( CMD_STRCMD );
		function_opackage::serialize_save( ar, fpk );
	}
};
*/
