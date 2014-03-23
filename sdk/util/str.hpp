#pragma once

#include <string>
#include <array>

static
std::string find_sub_string( const std::string& src, const std::string& left, const std::string& right, size_t& start )
{
	size_t pos1 = src.find( left, start );
	if( pos1 != std::string::npos )
	{
		pos1 += left.size();
		size_t pos2 = src.find( right, pos1 );
		if( pos2 != std::string::npos )
		{
			start = pos2 + right.size();
			return src.substr( pos1, pos2 - pos1 );
		}
	}
	return "";
}

static
std::string get_sub_string( const std::string& src, const std::string& left, const std::string& right )
{
	size_t start = 0;
	return find_sub_string( src, left, right, start );
}

static
std::string get_template_string( const std::string& src, const std::string& left, const std::string& right )
{
	size_t start = 0;
	return left + find_sub_string( src, left, right, start ) + right;
}

static
std::string bin2hex( const BYTE* pd, size_t len )
{
	std::string str;
	str.resize( len * 2 );
	for( size_t i = 0; i < len; ++i )
	{
		if( pd[i] >= 16 )
		{
			_ltoa( pd[i], &str[2 * i], 16 );
		}
		else
		{
			str[2 * i] = '0';
			_ltoa( pd[i], &str[2 * i + 1], 16 );
		}
	}
	return str;
}

inline
std::string bin2hex( const std::vector<BYTE>& src )
{
	if( src.empty() )
	{
		return std::string();
	}
	return bin2hex( &src[0], src.size() );
}
template<int N> inline
std::string bin2hex( const std::array<BYTE, N>& src )
{
	return bin2hex( &src[0], src.size() );
}

static
std::vector<BYTE> hex2bin( const char* src )
{
	std::vector<BYTE> bn;
	while( src[0] && src[1] )
	{
		char sz[3];
		sz[0] = src[0];
		sz[1] = src[1];
		sz[2] = 0;
		src += 2;
		BYTE v = ( BYTE )strtol( sz, nullptr, 16 );
		if( v == 0 )
		{
			if( sz[0] != '0' && sz[0] != '0' )
			{
				break;
			}
		}
		bn.push_back( v );
	}
	return bn;
}