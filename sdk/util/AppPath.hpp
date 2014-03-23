#pragma once

#include "path.hpp"

namespace detail
{
	template<typename char_type>
	struct api_GetModuleFileName
	{
		DWORD operator()( HMODULE, char_type*, DWORD );
	};
	inline DWORD api_GetModuleFileName<char>::operator()( HMODULE a0, char* a1, DWORD a2 )
	{
		return GetModuleFileNameA( a0, a1, a2 );
	};
	inline DWORD api_GetModuleFileName<wchar_t>::operator()( HMODULE a0, wchar_t* a1, DWORD a2 )
	{
		return GetModuleFileNameW( a0, a1, a2 );
	};

	//////////////////////////////////////////////////////////////////////////

	template < typename string_type,
			   typename lpfn_GetModuleFileName >
	static inline string_type GetAppPath( lpfn_GetModuleFileName fn_GetModuleFileName )
	{
		string_type r;
		r.resize( MAX_PATH );
		size_t len = fn_GetModuleFileName( 0, &r[0], MAX_PATH );
		r.resize( len );
		return r;
	}
};
//////////////////////////////////////////////////////////////////////////

template<typename string_type>
inline string_type GetAppPath()
{
	typedef detail::get_char_type<string_type>::type char_type;
	return detail::GetAppPath<string_type>( detail::api_GetModuleFileName<char_type>() );
}
//////////////////////////////////////////////////////////////////////////

template<typename string_type>
static inline string_type GetAppDir()
{
	return path_remove_filename( GetAppPath<string_type>() );
}

template<typename string_type>
static inline string_type GetAppFile( const string_type& file )
{
	return GetAppDir<string_type>() + file;
}

inline std::string GetAppFileA( const char* file )
{
	return GetAppFile<std::string>( file );
}

#define GetExecutePath GetAppPath
#define GetExecuteDir GetAppDir