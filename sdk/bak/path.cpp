#include "stdafx.h"
#include "path.h"

//#define USE_BOOST_DIRECTORY_ITERATOR 1

#if USE_BOOST_DIRECTORY_ITERATOR
#include <boost/filesystem/operations.hpp>
#else
#include <sdk/util/dir_enum.hpp>
#endif

void EnumFile( const bf::path& dir_path, bool childs, const std::function<bool( const bf::path&)>& pfn )
{
#if USE_BOOST_DIRECTORY_ITERATOR
	for( bf::directory_iterator it = bf::directory_iterator( dir_path ), it_end;
			it != it_end; ++it )
	{
		if( bf::is_regular( it->status() ) )
		{
			if( !pfn( it->path() ) )
			{
				break;
			}
		}
		else if( childs )
		{
			if( bf::is_directory( it->status() ) )
			{
				EnumFile( it->path(), childs, pfn );
			}
		}
	}
#else
	EnumDirectory( GetTPath( dir_path ).c_str(), childs, pfn );
#endif
}
