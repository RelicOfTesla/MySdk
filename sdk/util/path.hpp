#pragma once

#include <sdk/str_t.h>
#include <algorithm>

namespace detail
{
	template<typename char_type>
	struct api_ExpandEnvironmentStrings
	{
		DWORD operator()( const char_type*, char_type*, DWORD );
	};
	inline DWORD api_ExpandEnvironmentStrings<char>::operator()( const char* a0, char* a1, DWORD a2 )
	{
		return ExpandEnvironmentStringsA( a0, a1, a2 );
	}
	inline DWORD api_ExpandEnvironmentStrings<wchar_t>::operator()( const wchar_t* a0, wchar_t* a1, DWORD a2 )
	{
		return ExpandEnvironmentStringsW( a0, a1, a2 );
	}
	//
	template<typename char_type>
	struct api_GetFullPathName
	{
		BOOL operator()( const char_type*, DWORD, char_type*, char_type**);
	};
	inline BOOL api_GetFullPathName<char>::operator()( const char* a0, DWORD a1, char* a2, char** a3 )
	{
		return GetFullPathNameA( a0, a1, a2, a3 );
	}
	inline BOOL api_GetFullPathName<wchar_t>::operator()( const wchar_t* a0, DWORD a1, wchar_t* a2, wchar_t** a3 )
	{
		return GetFullPathNameW( a0, a1, a2, a3 );
	}
	//
	template<typename char_type>
	struct api_GetFileAttributes
	{
		DWORD operator()( const char_type*);
	};
	inline DWORD api_GetFileAttributes<char>::operator()( const char* a0 )
	{
		return GetFileAttributesA( a0 );
	}
	inline DWORD api_GetFileAttributes<wchar_t>::operator()( const wchar_t* a0 )
	{
		return GetFileAttributesW( a0 );
	}
	//////////////////////////////////////////////////////////////////////////

	template < typename string_type,
			   typename lpfn_ExpandEnvironmentStrings,
			   typename lpfn_GetFullPathName >
	static string_type path_system_complete( const string_type& filepath,
			lpfn_ExpandEnvironmentStrings fn_ExpandEnvironmentStrings,
			lpfn_GetFullPathName fn_GetFullPathName
										   )
	{
		UINT len;
		string_type expand_path;
		string_type result;

		char_traits<string_type>::resize( expand_path, MAX_PATH );
		len = fn_ExpandEnvironmentStrings( char_traits<string_type>::c_str( filepath ), &expand_path[0], MAX_PATH );
		char_traits<string_type>::resize( result, MAX_PATH );
		len = fn_GetFullPathName( char_traits<string_type>::c_str( expand_path ), MAX_PATH, &result[0], nullptr );
		char_traits<string_type>::resize( result, len );
		return result;
	}

	template < typename string_type,
			   typename lpfn_GetFileAttributes >
	static inline bool path_exists( const string_type& filepath, lpfn_GetFileAttributes fn_GetFileAttributes )
	{
		DWORD attribute = fn_GetFileAttributes( char_traits<string_type>::c_str( filepath ) );
		return attribute != DWORD( -1 );
	}
};
//////////////////////////////////////////////////////////////////////////

template<typename string_type>
inline bool path_exists( const string_type& filepath )
{
	typedef detail::get_char_type<string_type>::type char_type;
	return detail::path_exists( filepath, detail::api_GetFileAttributes<char_type>() );
}

template<typename string_type>
inline string_type path_system_complete( const string_type& filepath )
{
	typedef detail::get_char_type<string_type>::type char_type;
	return detail::path_system_complete( filepath,
										 detail::api_ExpandEnvironmentStrings<char_type>(),
										 detail::api_GetFullPathName<char_type>()
									   );
}
//////////////////////////////////////////////////////////////////////////
namespace detail
{
	template<typename iterator, typename char_type >
	inline iterator rfind_char( iterator first, iterator last, char_type ch )
	{
		return std::find_end( first, last, &ch, ( &ch ) + 1 );
	}
};

template<typename string_type>
const typename detail::get_char_type<string_type>::type* find_last_path_dir_char( const string_type& filepath )
{
	typedef detail::get_char_type<string_type>::type char_type;
	const char_type* pfirst = detail::char_traits<string_type>::c_str( filepath );
	size_t len = detail::char_traits<string_type>::size( filepath );
	const char_type* pend = pfirst + len;
	const char_type* iter1 = detail::rfind_char( pfirst, pend, char_type( '\\' ) );
	if( iter1 != pend )
	{
		pfirst = iter1;
	}
	const char_type* iter2 = detail::rfind_char( pfirst, pend, char_type( '/' ) );
	if( iter2 != pend )
	{
		return iter2;
	}
	if( iter1 != pend )
	{
		return iter1;
	}
	return nullptr;
}

template<typename string_type>
static inline string_type path_remove_filename( const string_type& filepath )
{
	typedef detail::get_char_type<string_type>::type char_type;
	const char_type* p = find_last_path_dir_char( filepath );
	if( p )
	{
		return filepath.substr( 0, p - &filepath[0] + 1 );
	}
	return filepath;
}

template<typename string_type>
static inline string_type path_filename( const string_type& filepath )
{
	typedef detail::get_char_type<string_type>::type char_type;
	const char_type* p = find_last_path_dir_char( filepath );
	if( p )
	{
		return &p[1];
	}
	return filepath;
}

#define path_get_directory path_remove_filename
