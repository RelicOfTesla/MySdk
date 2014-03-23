#pragma once
#include <sdk/win32_file.hpp>
#include "stream_t.hpp"

typedef CWinFile win32_fstream;

namespace archive
{
	template<>
	struct pod_archive<win32_fstream>
	{
		static void write( win32_fstream& stm, const pod_const_data& src )
		{
			stm.Write( src.data, src.len );
		}
		static void read( win32_fstream& stm, pod_buffer& dst )
		{
			if( stm.Read( dst.buffer, dst.buflen ) < dst.buflen )
			{
				throw std::runtime_error( "File EOF" );
			}
		}
	};
};

