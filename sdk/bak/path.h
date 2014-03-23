#pragma once

#include <boost/filesystem/path.hpp>
#include <sdk/strdef.h>

namespace bf = boost::filesystem;



void EnumFile( const bf::path& dir_path, bool childs, const std::function<bool( const bf::path&)>& pfn );

template<typename T> static
bool _AddRecord_OnEnumFile( const bf::path& filepath, T& result, const tstring& ext )
{
	if( ext.empty() || _tcsicmp( ext.c_str(), filepath.extension().c_str() ) == 0 )
	{
		result.push_back( filepath );
	}
	return true;
}

template<typename T> static
void GetFileListFromPath( T& result, const bf::path& dir_path, bool childs, const tstring& ext = TEXT( "" ) )
{
	EnumWinFile( dir_path, childs, std::bind( &_AddRecord_OnEnumFile<T>, std::placeholders::_1, std::ref( result ), std::ref( ext ) ) );
}

inline std::basic_string<TCHAR> GetTPath( const bf::path& filepath )
{
	return filepath.string< std::basic_string<TCHAR> >();
}