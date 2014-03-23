#pragma once

#include <sdk/binary_stream_base.hpp>
#include "stream_t.hpp"

namespace archive
{
	namespace _membase_t
	{
		template<typename Stream>
		struct pod_archive
		{
			static void read( Stream& stm, pod_buffer& dst )
			{
				stm.pop_buffer( ( PBYTE )dst.buffer, dst.buflen );
			}
			// 		static void read(Stream& stm, const pod_const_data& pb)
			// 		{
			// 			static_assert(0, "not support write");
			//  	}
		};
	};

	template<>
	struct pod_archive< memory_istream > : _membase_t::pod_archive<memory_istream>
	{};

	template<>
	struct pod_archive< binary_istream_ref > : _membase_t::pod_archive<binary_istream_ref>
	{};

	template<>
	struct pod_archive< binary_istream_new > : _membase_t::pod_archive<binary_istream_new>
	{};

	//////////////////////////////////////////////////////////////////////////

	template<>
	struct pod_archive< binary_ostream_new >
	{
		// 	static void read(binary_ostream& stm, pod_buffer& pb);
		// 	{
		// 		static_assert(0, "not support read");
		// 	}
		static void write( binary_ostream_new& stm, const pod_const_data& src )
		{
			stm.append( ( PBYTE )src.data, src.len );
		}
	};

	template<>
	struct pod_archive< binary_ostream_ref >
	{
		// 	static void read(binary_ostream& stm, pod_buffer& pb);
		// 	{
		// 		static_assert(0, "not support read");
		// 	}
		static void write( binary_ostream_ref& stm, const pod_const_data& src )
		{
			stm.append( ( PBYTE )src.data, src.len );
		}
	};
	//////////////////////////////////////////////////////////////////////////

	template<>
	struct pod_archive< binptr_ostream >
	{
		// 	static void read(binary_ostream& stm, pod_buffer& pb);
		// 	{
		// 		static_assert(0, "not support read");
		// 	}
		static void write( binptr_ostream& stm, const pod_const_data& src )
		{
			stm.append( ( PBYTE )src.data, src.len );
		}
	};

};
