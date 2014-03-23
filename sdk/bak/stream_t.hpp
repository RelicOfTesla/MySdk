#pragma once

#include <c++10/c++10_def.h>

template<typename Stream>
struct pod_archive;

template<typename Stream, typename T>
struct serialize_t;

struct serialize_flag
{
	enum
	{
		sflag = 1
	};
};

// template<>
// struct pod_archive< Stream >
// {
// 	static void invoke(Stream& stm, pod_buffer& pb);
// 	static void invoke(Stream& stm, const pod_const_data& pcd);
// };

// template<typename Stream>
// struct serialize_t<Stream, ctest>
// {
// 	template<typename Archive>
// 	static void invoke(Archive& ar, ctest&);
// };

namespace detail
{
	template<typename Stream>
	struct basic_archive_t
	{
		basic_archive_t( Stream& stm ) : m_stm( stm )
		{}

		inline Stream& get_stream()
		{
			return m_stm;
		}
		Stream& m_stm;
	private:
		basic_archive_t( const basic_archive_t&) {}
		void operator =( const basic_archive_t&) {}
	};


	template<typename Stream>
	struct iarchive_t : basic_archive_t<Stream>
	{
		enum
		{
			is_read = 1, is_load = 1,
			is_write = 0, is_save = 0,
		};

		iarchive_t( Stream& stm ) : basic_archive_t( stm )
		{}

		template<typename T>
		friend inline iarchive_t<Stream>& operator & ( iarchive_t<Stream>& stm, T& data )
		{
			stm >> data;
			return stm;
		};

		template<typename T>
		friend inline iarchive_t<Stream>& operator >> ( iarchive_t<Stream>& stm, T& data )
		{
			typedef get_serialize_t<Stream, T>::type serialize_impl;
			serialize_impl::invoke( stm, data );
			return stm;
		};

	};

	template<typename Stream>
	struct oarchive_t : basic_archive_t<Stream>
	{
		enum
		{
			is_read = 0, is_load = 0,
			is_write = 1, is_save = 1,
		};

		oarchive_t( Stream& stm ) : basic_archive_t( stm )
		{}

		template<typename T>
		friend inline oarchive_t<Stream>&  operator & ( oarchive_t<Stream>& stm, const T& data )
		{
			stm << data;
			return stm;
		}

		template<typename T>
		friend inline oarchive_t<Stream>& operator << ( oarchive_t<Stream>& stm, const T& data )
		{
			typedef get_serialize_t<Stream, T>::type serialize_impl;
			serialize_impl::invoke( stm, *const_cast<T*>( &data ) );
			return stm;
		}
	};

	struct pod_const_data
	{
		pod_const_data( const void* pv, size_t _len ) : data( pv ), len( _len )
		{}
		pod_const_data( const char szStr[] ) : data( szStr ), len( sizeof( szStr ) )
		{}

		const void* data;
		size_t len;
	};

	struct pod_buffer
	{
		pod_buffer( void* pv, size_t _len ) : buffer( pv ), buflen( _len )
		{}

		void* buffer;
		size_t buflen;
	};


	template<typename Stream, typename T>
	struct serialize_load_save : serialize_flag
	{
		struct call_read
		{
			template<typename Archive>
			static inline void invoke( Archive& ar, T& v )
			{
				serialize_t<Stream, T>::load( ar, v );
			}
		};
		struct call_write
		{
			template<typename Archive>
			static inline void invoke( Archive& ar, T& v )
			{
				serialize_t<Stream, T>::save( ar, v );
			}
		};

		template<typename Archive>
		static inline void invoke( Archive& ar, T& v )
		{
			typedef typename std::conditional<Archive::is_read, call_read, call_write>::type caller;
			caller::invoke( ar, v );
		}
	};

	template<typename Stream, typename T>
	struct pod_serialize_t : serialize_flag
	{
		template<typename Archive>
		static inline void invoke( Archive& ar, T& v )
		{
			static_assert( std::is_pod<T>::value, "only support pod type" );
			static_assert( !std::is_pointer<T>::value, "not support pointer" );

			typedef typename std::conditional<Archive::is_read, pod_buffer, pod_const_data>::type pod_type;
			pod_type pt( &v, sizeof( T ) );
#if !_DEBUG
			ar& pt;
#else
			typedef get_serialize_t<Stream, pod_type>::type serialize_impl;
			serialize_impl::invoke( ar, pt );
#endif
		}
	};

};

typedef detail::pod_buffer		pod_buffer;
typedef detail::pod_const_data	pod_const_data;

using detail::serialize_load_save;

template<typename Stream>
struct serialize_t<Stream, pod_buffer>
{
	template<typename Archive>
	static inline void invoke( Archive& ar, pod_buffer& v )
	{
		pod_archive<Stream>::invoke( ar.get_stream(), v );
	}
};

template<typename Stream>
struct serialize_t<Stream, pod_const_data>
{
	template<typename Archive>
	static inline void invoke( Archive& ar, pod_const_data& v )
	{
		pod_archive<Stream>::invoke( ar.get_stream(), v );
	}
};

template<typename Stream, typename T>
struct get_serialize_t
{
	static_assert( !std::is_pointer<T>::value, "not support pointer" );

	typedef typename std::conditional <
	std::is_pod <T >::value, detail::pod_serialize_t<Stream, T>, serialize_t<Stream, T>
	>::type type;

};

template<typename Stream, typename T>
struct serialize_t<Stream, T*>;
template<typename Stream, typename T>
struct serialize_t<Stream, const T*>;

template<typename Stream, int n>
struct serialize_t<Stream, char[n]>;
template<typename Stream, int n>
struct serialize_t<Stream, wchar_t[n]>;

template<typename Stream, int n>
struct serialize_t<Stream, const char[n]>;
template<typename Stream, int n>
struct serialize_t<Stream, const wchar_t[n]>;

namespace
{
	template<typename Stream, typename T>
	inline Stream& operator << ( Stream& stm, const T& v )
	{
		typedef detail::oarchive_t<Stream> Archive;

		typedef get_serialize_t<Stream, T>::type serialize_impl;

		static_cast< void( *)( Archive&, T&) >( serialize_impl::invoke );
		static_cast< void( *)( Stream&, const pod_const_data&) >( ::pod_archive<Stream>::invoke );

		Archive ar( stm );
		ar << v;
		return stm;
	}

	template<typename Stream, typename T>
	inline Stream& operator >> ( Stream& stm, T& v )
	{
		typedef detail::iarchive_t<Stream> Archive;

		typedef get_serialize_t<Stream, T>::type serialize_impl;

		static_cast< void( *)( Archive&, T&) >( serialize_impl::invoke );
		static_cast< void( *)( Stream&, pod_buffer&) >( ::pod_archive<Stream>::invoke );

		Archive ar( stm );
		ar >> v;
		return stm;
	}
};


namespace detail
{
	template<typename Stream, typename T>
	struct has_serialize_flag
	{
		template< int >
		struct is_find_type
		{
			char val[4];
		};
		struct not_find_type
		{
			char arr[8];
		};

		template<typename S, typename U>
		static is_find_type< get_serialize_t<S, U>::type::sflag  > test( int );
		template<typename S, typename U>
		static not_find_type test( ... );

		enum
		{
			nsize = sizeof( test<Stream, T> ( 0 ) ),
			value = ( nsize != sizeof( not_find_type ) )
		};
	};

};