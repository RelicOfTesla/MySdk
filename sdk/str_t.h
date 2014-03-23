#pragma once

#include <string>
#include <tchar.h>

typedef std::basic_string<TCHAR> tstring;
//////////////////////////////////////////////////////////////////////////
namespace detail
{
	template<typename T>
	struct char_traits;

	template<>
	struct char_traits<std::string> :  std::char_traits<char>
	{
		static inline const char* c_str( const std::string& s )
		{
			return s.c_str();
		}
		static inline size_t size( const std::string& s )
		{
			return s.size();
		}
		static inline void resize( std::string& s, size_t n )
		{
			s.resize( n );
		}

	};

	template<>
	struct char_traits<std::wstring> :  std::char_traits<wchar_t>
	{
		static inline const wchar_t* c_str( const std::wstring& s )
		{
			return s.c_str();
		}
		static inline size_t size( const std::wstring& s )
		{
			return s.size();
		}
		static inline void resize( std::wstring& s, size_t n )
		{
			s.resize( n );
		}

	};

	template<>
	struct char_traits<char> :  std::char_traits<char>
	{
		static inline const char* c_str( const char* s )
		{
			return s;
		}
		static inline size_t size( const char* s )
		{
			return strlen( s );
		}
	};

	template<>
	struct char_traits<wchar_t> :  std::char_traits<wchar_t>
	{
		static inline const wchar_t* c_str( const wchar_t* s )
		{
			return s;
		}
		static inline size_t size( const wchar_t* s )
		{
			return wcslen( s );
		}
	};

	template<typename T>
	struct char_traits<const T> : char_traits<T>
	{};
	template<typename T>
	struct char_traits<T&> : char_traits<T>
	{};
	template<typename T>
	struct char_traits<T*> : char_traits<T>
	{};
};


//////////////////////////////////////////////////////////////////////////

namespace detail
{
	template<typename type>
	struct get_char_type;

	template<>
	struct get_char_type<char>
	{
		typedef char type;
	};
	template<>
	struct get_char_type<wchar_t>
	{
		typedef wchar_t type;
	};
	template<>
	struct get_char_type<std::string>
	{
		typedef get_char_type<std::string::value_type>::type type;
	};
	template<>
	struct get_char_type<std::wstring>
	{
		typedef get_char_type<std::wstring::value_type>::type type;
	};
	template<typename T>
	struct get_char_type<T&>
	{
		typedef typename get_char_type<T>::type type;
	};
	template<typename T>
	struct get_char_type<const T>
	{
		typedef typename get_char_type<T>::type type;
	};

	template<typename T>
	struct get_char_type<T*>
	{
		typedef typename get_char_type<T>::type type;
	};
	template<typename T, int N>
	struct get_char_type<T[N]>
	{
		typedef typename get_char_type<T>::type type;
	};
};